
#include <vector>
#include <sstream>
#include <string>

#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include <sdf/Element.hh>

#include <gz/sim/components/DetachableJoint.hh>
#include "gz/sim/components/Link.hh"
#include "gz/sim/components/Model.hh"
#include "gz/sim/components/Name.hh"
#include "gz/sim/components/ParentEntity.hh"
#include "gz/sim/components/Pose.hh"
#include "gz/sim/Model.hh"
#include "gz/sim/Util.hh"

#include "AttachLinks.hh"

using namespace gz;
using namespace sim;
using namespace GZ_SIM_VERSION_NAMESPACE;
using namespace systems;

/////////////////////////////////////////////////
void AttachLinks::Configure(const Entity &_entity,
               const std::shared_ptr<const sdf::Element> &_sdf,
               EntityComponentManager &_ecm,
               EventManager &/*_eventMgr*/)
{
  this->model = Model(_entity);
  if (!this->model.Valid(_ecm))
  {
    gzerr << "AttachLinks should be attached to a model entity. "
           << "Failed to initialize." << std::endl;
    return;
  }

  if (_sdf->HasElement("parent"))
  {
    const auto parentLinkPath = _sdf->Get<std::string>("parent");

    // TODO deal with parent being world, this link is not part of the model

    this->parentLinkEntity = this->FindLinkByPath(_ecm, _entity, parentLinkPath);
    if (kNullEntity == this->parentLinkEntity)
    {
      gzerr << "Link with path " << parentLinkPath
             << " not found in model " << this->model.Name(_ecm)
             << ". Make sure the parameter 'parent' has the "
             << "correct value. Failed to initialize.\n";
      return;
    }
  }
  else
  {
    gzerr << "'parent' is a required parameter for AttachLinks. "
              "Failed to initialize.\n";
    return;
  }

  if (_sdf->HasElement("child"))
  {
    const auto childLinkPath = _sdf->Get<std::string>("child");
    this->childLinkEntity = this->FindLinkByPath(_ecm, _entity, childLinkPath);
    if (kNullEntity == this->childLinkEntity)
    {
      gzerr << "Link with path " << childLinkPath
             << " not found in model " << this->model.Name(_ecm)
             << ". Make sure the parameter 'child' has the "
             << "correct value. Failed to initialize.\n";
      return;
    }
  }
  else
  {
    gzerr << "'child' is a required parameter for AttachLinks."
              "Failed to initialize.\n";
    return;
  }

  // go and attach these links already
  this->fixed_joint = _ecm.CreateEntity();
  _ecm.CreateComponent(
      this->fixed_joint,
      components::DetachableJoint({this->parentLinkEntity,
                                   this->childLinkEntity, "fixed"}));

  this->validConfig = true;
}

//////////////////////////////////////////////////
void AttachLinks::PreUpdate(
  const UpdateInfo &/*_info*/,
  EntityComponentManager &_ecm)
{

}

GZ_ADD_PLUGIN(AttachLinks,
                    System,
                    AttachLinks::ISystemConfigure,
                    AttachLinks::ISystemPreUpdate)

//////////////////////////////////////////////////
Entity AttachLinks::FindLinkByPath(
  EntityComponentManager &_ecm,
  const Entity _modelEntity,
  const std::string &_path)
{
  // Check if we have a path with :: delimiters
  size_t pos = _path.find("::");
  if (pos == std::string::npos)
  {
    // Simple case: just a link name, use existing function
    Model modelObj(_modelEntity);
    return modelObj.LinkByName(_ecm, _path);
  }

  // We have a path with submodels
  std::string firstPart = _path.substr(0, pos);
  std::string remainingPath = _path.substr(pos + 2);
  
  // Find the submodel
  Model parentModel(_modelEntity);
  Entity submodelEntity = kNullEntity;
  
  // Find all nested models
  _ecm.Each<components::Model, components::Name, components::ParentEntity>(
    [&](const Entity &_entity,
        const components::Model *,
        const components::Name *_name,
        const components::ParentEntity *_parent) -> bool
    {
      // Check if this is a direct child model of our parent model
      if (_parent->Data() == _modelEntity && _name->Data() == firstPart)
      {
        submodelEntity = _entity;
        return false;  // Stop searching once found
      }
      return true;
    });
  
  if (submodelEntity == kNullEntity)
  {
    // Submodel not found
    gzerr << "Submodel '" << firstPart << "' not found in model path '" 
           << _path << "'." << std::endl;
    return kNullEntity;
  }
  
  // Now we've found the submodel, continue to resolve the rest of the path
  pos = remainingPath.find("::");
  if (pos == std::string::npos)
  {
    // This is the final link
    Model submodel(submodelEntity);
    return submodel.LinkByName(_ecm, remainingPath);
  }
  else
  {
    // More submodels to traverse
    return FindLinkByPath(_ecm, submodelEntity, remainingPath);
  }
}

GZ_ADD_PLUGIN_ALIAS(AttachLinks,
  "gz::sim::systems::AttachLinks")


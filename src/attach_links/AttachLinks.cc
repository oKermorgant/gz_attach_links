
#include <vector>

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
    const auto parentLinkName = _sdf->Get<std::string>("parent");

    // TODO deal with parent being world, this link is not part of the model

    this->parentLinkEntity = this->model.LinkByName(_ecm, parentLinkName);
    if (kNullEntity == this->parentLinkEntity)
    {
      gzerr << "Link with name " << parentLinkName
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
    const auto childLinkName = _sdf->Get<std::string>("child");
    this->childLinkEntity = this->model.LinkByName(_ecm, childLinkName);
    if (kNullEntity == this->childLinkEntity)
    {
      gzerr << "Link with name " << childLinkName
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

GZ_ADD_PLUGIN_ALIAS(AttachLinks,
  "gz::sim::systems::AttachLinks")


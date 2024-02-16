
#ifndef GZ_SIM_SYSTEMS_ATTACHLINKS_HH_
#define GZ_SIM_SYSTEMS_ATTACHLINKS_HH_

#include <memory>
#include <string>

#include "gz/sim/Model.hh"
#include "gz/sim/System.hh"

namespace gz
{
namespace sim
{
// Inline bracket to help doxygen filtering.
inline namespace GZ_SIM_VERSION_NAMESPACE {
namespace systems
{
  /// \brief A system that initially attaches two links via a fixed joint
  ///
  /// ## System Parameters
  ///
  /// - `<parent>`: Name of the link that will be considered the parent.
  ///
  /// - `<child>`: Name of the link that will be considered the child.

  class AttachLinks
      : public System,
        public ISystemConfigure,
        public ISystemPreUpdate
  {
    /// Documentation inherited
    public: AttachLinks() = default;

    /// Documentation inherited
    public: void Configure(const Entity &_entity,
                           const std::shared_ptr<const sdf::Element> &_sdf,
                           EntityComponentManager &_ecm,
                           EventManager &_eventMgr) final;

    /// Documentation inherited
    public: void PreUpdate(
                const gz::sim::UpdateInfo &_info,
                gz::sim::EntityComponentManager &_ecm) final;

    /// \brief The model associated with this system.
    private: Model model;

    /// \brief the fixed joint that is created on spawn
    private: Entity fixed_joint;

    /// \brief Entity of attachment link in the parent model
    private: Entity parentLinkEntity{kNullEntity};

    /// \brief Entity of attachment link in the child model
    private: Entity childLinkEntity{kNullEntity};

    /// \brief Whether all parameters are valid and the system can proceed
    private: bool validConfig{false};

  };
  }
}
}
}

#endif

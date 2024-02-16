# Closed kinematic chains in Gazebo Sim

This package shows how to get a parallel robot working in Gazebo Sim.

## Overall approach

Contrary to Gazebo classic, Gazebo Sim does not handle closed kinematic chains *in the SDF file*.
Actually what is not authorized is a link having two parents (which is necessary for a closed chain, but may also be a non-closed chain).

In order to obtain a closed chain, the SDF must thus describe the robot as a tree, where all links are correctly placed.

A plugin is then used to attach two links and close the loop at runtime.

### DetachableJoint

This plugin is already part of Gazebo and is designed to attach or detach a joint between two models. In our case we only attach it, and we use it on the same model:

```xml
<plugin filename="libgz-sim-detachable-joint-system.so" name="gz::sim::systems::DetachableJoint">
  <parent_link>link_CD</parent_link>
  <child_model>four_bar</child_model>  <!-- of course I know him, it's me -->
  <child_link>link_CD_closing</child_link>
  <attach_topic>close_loop</attach_topic>
</plugin>
```

### AttachLinks

A custom plugin is also provided in this package, namely `AttachLinks`. This one only takes the two link names:

```xml
<!--plugin filename="libattach-links.so" name="gz::sim::systems::AttachLinks">
  <parent>link_CD</parent>
  <child>link_CD_closing</child>
</plugin-->
```

## Robot examples

Two examples are given, see the `model` folder and the launch file.

- a five bar from the [Gazebo classic example](https://classic.gazebosim.org/tutorials?tut=kinematic_loop&cat=)

![](images/five_bar.gif)

- and a biglide that we use for teaching

![](images/biglide.gif)

## Running the examples

The provided launch file requires `simple_launch` and `slider_publisher`.

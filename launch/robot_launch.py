from simple_launch import SimpleLauncher, GazeboBridge

sl = SimpleLauncher(use_sim_time = True)

sl.declare_arg('sliders',default_value=True)
sl.declare_arg('robot', default_value='four_bar')


def launch_setup():

    robot = sl.arg('robot')

    if robot == 'biglide':
        joints = ['A12', 'A21']
    else:
        joints = ['joint_A', 'joint_D']

    sl.spawn_gz_model(name = robot, model_file = sl.find('gz_attach_links', robot + '.sdf'),
                      spawn_args=['-z', '0.5'])

    # ROS-Gz bridges
    bridges = [GazeboBridge.clock()]

    # joint states
    bridges.append(GazeboBridge(GazeboBridge.model_prefix(robot) + '/joint_state',
                                'joint_states',
                                'sensor_msgs/JointState', GazeboBridge.gz2ros))

    for i,joint in enumerate(joints):
        bridges.append(GazeboBridge(f'/model/{robot}/joint/{joint}/cmd_force',
                                            f'joint{i+1}_cmd_effort',
                                            'std_msgs/Float64',
                                            GazeboBridge.ros2gz))
    sl.create_gz_bridge(bridges)

    if sl.arg('sliders'):
        sl.node('slider_publisher', arguments=[sl.find('gz_attach_links', 'effort_manual.yaml')])
    
    return sl.launch_description()


generate_launch_description = sl.launch_description(launch_setup)

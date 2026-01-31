from simple_launch import SimpleLauncher, GazeboBridge
import subprocess as sp

sl = SimpleLauncher(use_sim_time = True)
sl.declare_arg('sliders',default_value=True)
sl.declare_arg('robot', default_value='biglide')

def launch_setup():

    robot = sl.arg('robot')
    model_file = sl.find('gz_attach_links') + f'/models/{robot}/{robot}.sdf'

    # parses the file and breaks the loops
    # returns the modified sdf path if any loops
    model_file = sp.run(f'ros2 run gz_attach_links unroll_loops -f {model_file} -n {robot}'.split(),
                      stdout=sp.PIPE).stdout.decode()

    print('Spawning',model_file)

    sl.spawn_gz_model(name = robot,
                      model_file = model_file,
                      spawn_args=['-z', '0.5'])

    # ROS-Gz bridges
    bridges = [GazeboBridge.clock()]

    # joint states
    bridges.append((GazeboBridge.model_prefix(robot) + '/joint_state',
                    'joint_states',
                    'sensor_msgs/JointState',
                    GazeboBridge.gz2ros))

    # joint command
    for i,joint in enumerate(('A11', 'A21')):
        bridges.append((f'/model/{robot}/joint/{joint}/cmd_force',
                        f'joint{i+1}_cmd_effort',
                        'std_msgs/Float64',
                        GazeboBridge.ros2gz))

    sl.create_gz_bridge(bridges)

    if sl.arg('sliders'):
        sl.node('slider_publisher', arguments=[sl.find('gz_attach_links', 'effort_manual.yaml')])
    
    return sl.launch_description()


generate_launch_description = sl.launch_description(launch_setup)

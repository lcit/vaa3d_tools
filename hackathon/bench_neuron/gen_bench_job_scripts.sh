#!/bin/bash
#
#This is a shell program to batch reconstruct images using five different methods, including APP1, APP2, MOST, neuTube, snake, simpletracing, and TReMap.
#APP1,APP2,MOST, and simpletracing are built together with Vaa3D
#
#To build neuTube plugin, 1. go to /vaa3d_tools/released_plugins/v3d_plugins/neurontracing_neutube/src_neutube/, and run 
#	                                     sh build.sh <qmake_path> <qmake_spec_path>
#	              more detailed information can be found in /local1/work/vaa3d_tools/released_plugins/v3d_plugins/neurontracing_neutube/how_to_build
#		   2. go to /local1/work/vaa3d_tools/hackathon/zhi/neuTube_zhi/, and run qmake & make
#
#To build FarSight Snake Tracing plugin, please check the README file in /vaa3d_tools/hackathon/zhi/snake_tracing/README
#
#
# One example to call this script:
#    for j in `seq 1 7`; do echo $j; for i in $(ls *g.v3draw); do echo $i; ~/work/vaa3d_tools/hackathon/bench_neuron/gen_bench_job_scripts.sh $j /local2/workdata/bench788/smoothed_8bit/$i /home/hanchuanp/tmp ; done; done;
#
# by Hanchuan Peng
# 2014-Nov-1
#

function write_vaa3d_job_config {

  outputScript=$1;

  echo "#!/bin/bash" >> $outputScript;
  echo "#" >> $outputScript;
  echo "#" >> $outputScript;

  echo "## Check which queue you may use" >> $outputScript;
#  echo "#PBS -q dque" >> $outputScript;
  echo "#PBS -q mindscope@qmaster" >> $outputScript;
  echo "# Declare that your job will use no more than some amount of memory _at_peak_" >> $outputScript;
  echo "#PBS -l vmem=16g" >> $outputScript;
  echo "# Allow up to 10min of walltime.  Default is 12 hours" >> $outputScript;
  echo "#PBS -l walltime=00:10:00" >> $outputScript;
  echo "# Request just one core on the host" >> $outputScript;
  echo "#PBS -l ncpus=1" >> $outputScript;
  echo "# Give your job a descriptive name. This is visible in qstat and other job reports.  Also serves as the default basename for log files" >> $outputScript;
#  echo "#PBS -N ${outputScript}" >> $outputScript;
  echo "#PBS -N /data/aibstemp/hanchuan_tmp/1.txt" >> $outputScript;
  echo "# Should torque automatically re-run the job on error?" >> $outputScript;
  echo "#PBS -r n" >> $outputScript;
  echo "# Merge STDOUT into STDERR" >> $outputScript;
  echo "#PBS -j oe" >> $outputScript;
  echo "# location for stderr/stdout log files _after_ job completion" >> $outputScript;
  echo "#PBS -o ${outputScript}.out" >> $outputScript;

  echo "#" >> $outputScript;
  echo "#" >> $outputScript;

#  echo "# send email on job error" >> $outputScript;
#  echo "#PBS -m a" >> $outputScript;

  DISPLAY1=:$RANDOM;
  echo "export DISPLAY=$DISPLAY1" >> $outputScript;
  echo "Xvfb $DISPLAY1 -auth /dev/null &" >> $outputScript;
#  echo "export LD_PRELOAD=/usr/lib64/libstdc++.so.6" >> $outputScript;

  echo "export LD_LIBRARY_PATH=/home/hanchuanp/work/v3d_external/bin" >> $outputScript;

}

function write_neuron_tracing_command {

  outputScript=$1;
  METHOD=$2;
  vaa3dProgramPath=$3;
  inimgfileTracing=$4;
  finalfileFolder=$5;

  smooth_inimgfileTracing=${inimgfileTracing}.g.v3draw;

#smooth image only

  if [ $METHOD == "smooth" ]; then
   echo "$vaa3dProgramPath -x gaussian -f gf -i $inimgfileTracing -o $smooth_inimgfileTracing -p 7 7 2 1 2" >> $outputScript;
   echo "$vaa3dProgramPath -x datatypeconvert -f dtc -i $smooth_inimgfileTracing -o $smooth_inimgfileTracing -p 1" >> $outputScript;
   echo "mv  $smooth_inimgfileTracing $finalfileFolder" >> $outputScript;
  fi;

#APP1

  if [ $METHOD == "1" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x vn2 -f app1 -i $inimgfileTracing -p NULL 0 40 0" >> $outputScript;
    echo "mv  $inimgfileTracing*_app1.swc $finalfileFolder" >> $outputScript;
  fi;

#APP2

  if [ $METHOD == "2" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x vn2 -f app2 -i $inimgfileTracing -p NULL 0 10 1 1 1 0 5" >> $outputScript;
    echo "mv  $inimgfileTracing*_app2.swc $finalfileFolder" >> $outputScript;
    echo "rm  $inimgfileTracing*_ini.swc" >> $outputScript;
  fi;

#MOST

  if [ $METHOD == "3" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x MOST -f MOST_trace -i $inimgfileTracing -p 1 40" >> $outputScript;
    echo "mv  $inimgfileTracing*_MOST.swc $finalfileFolder" >> $outputScript;
  fi;

#NEUTUBE

  if [ $METHOD == "4" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x neuTube -f neutube_trace -i $inimgfileTracing -p 1 1" >> $outputScript;
    echo "mv  $inimgfileTracing*_neutube.swc $finalfileFolder" >> $outputScript;
  fi;

#FARSIGHT Snake

  if [ $METHOD == "5" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x snake -f snake_trace -i $inimgfileTracing -p 1" >> $outputScript;
    echo "mv  $inimgfileTracing*_snake.swc $finalfileFolder" >> $outputScript;
  fi;

#3 from SimpleTracing

  if [ $METHOD == "6" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x SimpleTracing -f tracing -i $inimgfileTracing -o ${inimgfileTracing}_simple.swc -p 1" >> $outputScript;
    echo "mv  $inimgfileTracing*_simple.swc $finalfileFolder" >> $outputScript;
  fi;

  if [ $METHOD == "7" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x SimpleTracing -f ray_shooting -i $inimgfileTracing -o ${inimgfileTracing}_Rayshooting.swc" >> $outputScript;
    echo "mv  $inimgfileTracing*_Rayshooting.swc $finalfileFolder" >> $outputScript;
  fi;

  if [ $METHOD == "8" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x SimpleTracing -f dfs -i $inimgfileTracing -o ${inimgfileTracing}_Rollerball.swc" >> $outputScript;
    echo "mv  $inimgfileTracing*_Rollerball.swc $finalfileFolder" >> $outputScript;
  fi;

#TreMap

  if [ $METHOD == "9" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x TReMap -f trace_mip -i $inimgfileTracing -p 0 1 10 0 1 0 5" >> $outputScript;
    echo "mv  $inimgfileTracing*_TreMap.swc $finalfileFolder" >> $outputScript;
  fi;

#MST

  if [ $METHOD == "10" -o $METHOD == "-1" ]; then
    echo "$vaa3dProgramPath -x MST_tracing -f trace_mst -i $inimgfileTracing -p 1 5" >> $outputScript;
    echo "mv  $inimgfileTracing*_MST_Tracing.swc $finalfileFolder" >> $outputScript;
  fi;

}

#copy the names
tracingMethod=$1
inputImgFile=$2
finalfileFolder=$3
vaa3dProgramPath=/home/hanchuanp/work/v3d_external/bin/start_vaa3d.sh

jobScriptFile=${inputImgFile}_${tracingMethod}.sh

#generate the batch script configuration
if [ -f $jobScriptFile ]; then
  rm $jobScriptFile;
fi;

write_vaa3d_job_config $jobScriptFile

#generate the actual tracing code
if [ ! -d $finalfileFolder ]; then
  mkdir $finalfileFolder
fi

write_neuron_tracing_command $jobScriptFile $tracingMethod $vaa3dProgramPath $inputImgFile $finalfileFolder

# precisely kill the job

echo "kill %1" >> $jobScriptFile;



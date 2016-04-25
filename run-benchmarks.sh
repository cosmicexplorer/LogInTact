#!/bin/sh

outfile=bench.out
echo 'parallel_type,threads,chunk_size,num_chunks,sched_type,time' > "$outfile"

export TIMEFORMAT='%E'

echo rm -f bench-std{out,err}
rm -f bench-std{out,err}

run_with_params() {
  out_filename="outfile-$1-$2-$3-$4-$5.out"
  make clean
  rm -f "$out_filename"
  echo make RELEASE=1 PARALLEL_METHOD="$1" CHUNK_SIZE="$2" CHUNK_NUM="$3" \
       OMP_SCHEDULE_TYPE="$5" NUM_THREADS=$4
  make RELEASE=1 PARALLEL_METHOD="$1" CHUNK_SIZE="$2" CHUNK_NUM="$3" \
       OMP_SCHEDULE_TYPE="$5" NUM_THREADS="$4"
  echo ./sim "$out_filename"
  echo -n "$1,$4,$2,$3,$5," >>"$outfile"
  # filesize="$(echo '76' '*' "$2" '*' "$3" | bc)"
  (time (./sim "$out_filename" >>bench-stdout 2>>bench-stderr) \
        2>>"$outfile")
  # realfilesize="$(stat --printf="%s" "$out_filename")"
  # if [ "$realfilesize" != "$filesize" ]; then
  #   echo "real size $realfilesize != expected size $filesize" 1>&2
  #   exit 1
  # fi
}

num_chunks=50

for chunk_size in 100000 250000 500000 1000000; do
  # serial
  run_with_params NONE "$chunk_size" "$num_chunks" NA NA
  # openmp!
  for sched in static dynamic guided; do
    for num_threads in 1 2 4 8 16; do
      run_with_params OPENMP "$chunk_size" "$num_chunks" "$num_threads" "$sched"
    done
  done
done

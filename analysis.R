library(ggplot2)

outfile <- read.csv(
    'bench.out',
    colClasses=c('character','integer','integer','integer','character','double'))

serial <- outfile[outfile$parallel_type == 'NONE',]
openmp <- outfile[outfile$parallel_type == 'OPENMP',]
openmp$speedup <- serial[serial$chunk_size == openmp$chunk_size,]$time[1] / openmp$time

## slowdown charts
png('serial-slowdown.png', width=500, height=500, type='cairo')
ggplot(serial, aes(x=chunk_size,y=time)) +
    geom_line() +
    xlab('Problem Size') +
    ylab('Time Taken (s)') +
    ggtitle('Slowdown of Linear Program By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('parallel-slowdown-static.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'static',], aes(x=chunk_size,y=time)) +
    geom_line(color=factor(threads)) +
    xlab('Problem Size') +
    ylab('Time Taken (s)') +
    labs(color='Number of Threads') +
    ggtitle('Slowdown of OpenMP Static Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('parallel-slowdown-dynamic.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'dynamic',], aes(x=chunk_size,y=time)) +
    geom_line(color=factor(threads)) +
    xlab('Problem Size') +
    ylab('Time Taken (s)') +
    labs(color='Number of Threads') +
    ggtitle('Slowdown of OpenMP Dynamic Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('parallel-slowdown-guided.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'guided',], aes(x=chunk_size,y=time)) +
    geom_line(color=factor(threads)) +
    xlab('Problem Size') +
    ylab('Time Taken (s)') +
    labs(color='Number of Threads') +
    ggtitle('Slowdown of OpenMP Guided Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

## speedup charts
png('static-speedup.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'static',], aes(x=chunk_size,y=speedup)) +
    geom_line(color=factor(threads)) +
    xlab('Problem Size') +
    ylab('Speedup Over Serial Version') +
    labs(color='Number of Threads') +
    ggtitle('Speedup of OpenMP Static Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('dynamic-speedup.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'dynamic',], aes(x=chunk_size,y=speedup)) +
    geom_line(color=factor(threads)) +
    xlab('Problem Size') +
    ylab('Speedup Over Serial Version') +
    labs(color='Number of Threads') +
    ggtitle('Speedup of OpenMP Dyanmic Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('guided-speedup.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'guided',], aes(x=chunk_size,y=speedup)) +
    geom_line(color=factor(threads)) +
    xlab('Problem Size') +
    ylab('Speedup Over Serial Version') +
    labs(color='Number of Threads') +
    ggtitle('Speedup of OpenMP Guided Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

## speedup combined
png('parallel-speedup.png', width=500, height=500, type='cairo')
ggplot(openmp, aes(x=threads,y=speedup)) +
    geom_line(color=factor(sched_type)) +
    xlab('Problem Size') +
    ylab('Speedup Over Serial Version') +
    labs(color='Schedule Type') +
    ggtitle('Speedup of OpenMP Static Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

library(ggplot2)

outfile <- read.csv(
    'bench.out',
    colClasses=c('character','integer','integer','integer','character','double'))

serial <- outfile[outfile$parallel_type == 'NONE',]
openmp <- outfile[outfile$parallel_type == 'OPENMP',]
for (i in 1:dim(openmp)[1]) {
    el <- openmp[i,]
    openmp$speedup[i] <- serial[serial$chunk_size == el$chunk_size,]$time[1] / el$time
}

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
    geom_line(aes(color=factor(threads))) +
    xlab('Problem Size') +
    ylab('Time Taken (s)') +
    labs(color='Number of Threads') +
    ggtitle('Slowdown of OpenMP Static Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('parallel-slowdown-dynamic.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'dynamic',], aes(x=chunk_size,y=time)) +
    geom_line(aes(color=factor(threads))) +
    xlab('Problem Size') +
    ylab('Time Taken (s)') +
    labs(color='Number of Threads') +
    ggtitle('Slowdown of OpenMP Dynamic Schedule By Problem Size') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('parallel-slowdown-guided.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'guided',], aes(x=chunk_size,y=time)) +
    geom_line(aes(color=factor(threads))) +
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
    geom_line(aes(color=factor(threads))) +
    xlab('Problem Size') +
    ylab('Speedup Over Serial Version') +
    labs(color='Number of Threads') +
    ggtitle('Speedup of OpenMP Static Schedule By Number of Threads') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('dynamic-speedup.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'dynamic',], aes(x=chunk_size,y=speedup)) +
    geom_line(aes(color=factor(threads))) +
    xlab('Problem Size') +
    ylab('Speedup Over Serial Version') +
    labs(color='Number of Threads') +
    ggtitle('Speedup of OpenMP Dynamic Schedule By Number of Threads') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

png('guided-speedup.png', width=500, height=500, type='cairo')
ggplot(openmp[openmp$sched_type == 'guided',], aes(x=chunk_size,y=speedup)) +
    geom_line(aes(color=factor(threads))) +
    xlab('Problem Size') +
    ylab('Speedup Over Serial Version') +
    labs(color='Number of Threads') +
    ggtitle('Speedup of OpenMP Guided Schedule By Number of Threads') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

## speedup combined
png('parallel-speedup.png', width=500, height=500, type='cairo')

averages <- vector()
for (i in 1:dim(openmp)[1]) {
    el <- openmp[i,]
    averages[i] <- mean(openmp[openmp$threads == el$threads &
                               openmp$sched_type == el$sched_type,]$speedup)
}

ggplot(openmp, aes(x=threads, y=averages)) +
    geom_line(aes(color=factor(sched_type))) +
    xlab('Number of Threads') +
    ylab('Speedup Over Serial Version') +
    labs(color='Schedule Type') +
    ggtitle('Speedup of Different OpenMP Schedules By Number of Threads') +
    scale_x_continuous(expand=c(0,0)) +
    scale_y_continuous(expand=c(0,0))
dev.off()

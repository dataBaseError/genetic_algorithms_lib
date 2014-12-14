###############################################################################
#
# Performs statistical analysis of the benchmark data for multi-threaded
# N-Queens implementation and generates plots of the results
# 
# Copyright (C) 2014, Jonathan Gillett & Joseph Heron
# All rights reserved.
#
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################
library(stringr)
library(data.table)
library(psych)
library(ggplot2)
library(RColorBrewer)


figure_dir <- "/home/jon/Source/RESEARCH/genetic_algorithms_lib/analysis/plots/"


# Parse the results
output <- data.table(output_12132014_1443_fixed)
output <- output[, list(trial, queens=as.factor(queens), maxgen=as.factor(maxgen), 
                        competitors=as.factor(competitors), threads=as.factor(threads),
                        solved, time=as.numeric(time))]
setkey(output, queens, maxgen, competitors, threads)



# Get the average running time for each n-queens, each maxgen, each threads
runtime <- output[, list(time=mean(time)), by="queens,maxgen,competitors,threads"]

# Get the average running time of each n-queens, for each num threads
# make a plot Y is time, X is threads, colour is num queens
p <- ggplot(runtime[maxgen==100000 & competitors==1], aes(x=threads, y=time, colour=factor(queens)))
p <- p + geom_line(size=1.75) + 
    scale_x_continuous(breaks=1:10) + scale_y_continuous() +
    scale_colour_manual(values = c(brewer.pal(3, "Set2"))) +
    labs(x = "Threads", y = "Time (Seconds)", 
         title = "Execution Time - No Competitive Learning, Max 100,000 Generations",
         colour = "Queens")

file <- "time_nocomp_100k.png"
file <- paste(figure_dir, file, sep="/")
ggsave(filename=file, plot=p, width=12, height=6)


p <- ggplot(runtime[maxgen==1000000 & competitors==1], aes(x=threads, y=time, colour=factor(queens)))
p <- p + geom_line(size=1.75) + 
    scale_x_continuous(breaks=1:10) + scale_y_continuous() +
    scale_colour_manual(values = c(brewer.pal(3, "Set2"))) +
    labs(x = "Threads", y = "Time (Seconds)", 
         title = "Execution Time - No Competitive Learning, Max 1,000,000 Generations",
         colour = "Queens")

file <- "time_nocomp_1m.png"
file <- paste(figure_dir, file, sep="/")
ggsave(filename=file, plot=p, width=12, height=6)





# Show the improvement irrespective of threads once we optimized the GA technique
p <- ggplot(runtime[maxgen==1000000 & competitors==2], aes(x=threads, y=time, colour=factor(queens)))
p <- p + geom_line(size=1.75) + 
    scale_x_continuous(breaks=1:10) + scale_y_continuous() +
    scale_colour_manual(values = c(brewer.pal(3, "Set2"))) +
    labs(x = "Threads", y = "Time (Seconds)", 
         title = "Execution Time - Competitive Learning with 2 Populations",
         colour = "Queens")

file <- "time_2comp_1m.png"
file <- paste(figure_dir, file, sep="/")
ggsave(filename=file, plot=p, width=12, height=6)


p <- ggplot(runtime[maxgen==1000000 & competitors==3], aes(x=threads, y=time, colour=factor(queens)))
p <- p + geom_line(size=1.75) + 
    scale_x_continuous(breaks=1:10) + scale_y_continuous() +
    scale_colour_manual(values = c(brewer.pal(3, "Set2"))) +
    labs(x = "Threads", y = "Time (Seconds)", 
         title = "Execution Time - Competitive Learning with 3 Populations",
         colour = "Queens")

file <- "time_3comp_1m.png"
file <- paste(figure_dir, file, sep="/")
ggsave(filename=file, plot=p, width=12, height=6)




# Get the statistics for the average number of trials that found a solution
# within the limit of the maximum number of generations
solved <- output[, list(solved=sum(solved)), by="queens,maxgen,competitors,threads"]
solved <- solved[, list(solved=mean(solved)), by="queens,maxgen,competitors"]


# Show how improving the implementation increased the likelihood of finding a solution
p <- ggplot(data=solved[maxgen==1000], aes(x=factor(queens), y=solved))
p <- p + geom_bar(aes(fill=competitors), stat="identity", position=position_dodge()) +
    scale_fill_manual(values = c(brewer.pal(3, "Set2"))) + 
    labs(x = "Queens", y = "Times a solution was found", fill = "Populations",
         title = "Times a Solution was Found, Max 1,000 Generations")

file <- "sol_comp_1k.png"
file <- paste(figure_dir, file, sep="/")
ggsave(filename=file, plot=p, width=12, height=6)


p <- ggplot(data=solved[maxgen==10000], aes(x=factor(queens), y=solved))
p <- p + geom_bar(aes(fill=competitors), stat="identity", position=position_dodge()) +
    scale_fill_manual(values = c(brewer.pal(3, "Set2"))) + 
    labs(x = "Queens", y = "Times a solution was found", fill = "Populations",
         title = "Times a Solution was Found, Max 10,000 Generations")

file <- "sol_comp_10k.png"
file <- paste(figure_dir, file, sep="/")
ggsave(filename=file, plot=p, width=12, height=6)
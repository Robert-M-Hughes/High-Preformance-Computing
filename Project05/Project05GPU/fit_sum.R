A <- read.csv("10240kernel.csv", header=TRUE);
hor<-A$Horizontal.Convolution;
ver<-A$Vertical.Convolution;
mag <- A$Magnitude.Kernel;
dir <-A$Gradient.Kernel;
thr <-A$Thrust.Sorting;
sup <- A$Suppression.Kernel;
hys <-A$Hysteresis.Kernel;
edg <-A$Edge.Linking.Kernel;


fit<-lm(hor~bsize)
summary(fit)

fit<-lm(ver~bsize)
summary(fit)

fit<-lm(mag~bsize)
summary(fit)

fit<-lm(dir~bsize)
summary(fit)


fit<-lm(sup~bsize)
summary(fit)

fit<-lm(thr~bsize)
summary(fit)

fit<-lm(hys~bsize)
summary(fit)

fit<-lm(edg~bsize)
summary(fit)

A <- read.csv('dim32Kernel.csv', header = TRUE)
bsize<-A$Image.Size;
hor<-A$Horizontal.Convolution;
ver<-A$Vertical.Convolution;
mag <- A$Magnitude.Kernel;
dir <-A$Gradient.Kernel;
thr <-A$Thrust.Sorting;
sup <- A$Suppression.Kernel;
hys <-A$Hysteresis.Kernel;
edg <-A$Edge.Linking.Kernel;
newdata = data.frame(bsize=3072)
newdata1 = data.frame(bsize=5170)
newdata2 = data.frame(bsize=7680)


plot(edg~bsize)
fit<-lm(edg~bsize)
summary(fit)
abline(fit)
predict(fit, newdata, interval="predict")
predict(fit, newdata1, interval="predict")
predict(fit, newdata2, interval="predict")
A <- read.csv('avg.csv', header = TRUE)
A

size <- A$KILO
kh2d <- A$h2d
kd2h<- A$d2h
mh2d <- A$h2d
md2h <- A$d2h

newdata = data.frame(bsize=9437184/1000000)
newdata1 = data.frame(bsize=26728900/1000000)
newdata2 = data.frame(bsize=58982400/1000000)

plot(md2h ~ size)
fit <- lm(md2h ~ log(sie))
summary(fit)
abline(fit)

predict(fit, newdata, interval="predict")
predict(fit, newdata1, interval="predict")
predict(fit, newdata2, interval="predict")
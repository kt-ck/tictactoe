#tictactoe

##关于Q-learning
Q-learning 采用min-max训练，导致无法应对每一步不是最优解的情况，需要和人直接对战，才可以训练出较好的模型。

##运行
进入Debug,运行exe文件即可

##说明
对战暂时只能在同一个局域网内，由于不能获得外网ip。在设计的时候，游戏设计成有一方是server，导致游戏双方通信地位不平等，难以采用服务器中转的方式，实现不在同一个局域网内的匹配。
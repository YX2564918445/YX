import random
from typing import Tuple, Union

import numpy as np


from V5RPC import *
import math
from baseRobot import *
from GlobalVariable import *

baseRobots = []                     # 定义我方机器人数组
oppRobots = []                      # 定义对方机器人数组
data_loader = DataLoader()
race_state = -1                     # 定位球状态
race_state_trigger = -1             # 触发方
num_goalkepper = -1                 #对方守门员编号 1代表有守门员  其余为有

penalty = 1                         #点球的决策树
flag_penalty = -1                   #如果为1 则换点球策略
defend_flag = 0                     #0为正常防守  1为点球防守
tick = 1                            #点球防守策略树
flag_tick = -1                      #如果为1 则换策略
goal = 1                            #门球的决策树
flag_goal = -1                      #如果为1 则换门球策略
last_race_state = -1                #上一次比赛情况
last_race_state_trigger = -1        #上一次触发方
guard_goal = 1                      #门球防守的决策树  防止被对方造犯规
faceoff  = 1                        # 开球策略树
flag_faceoff = -1                   # 如果为1 则换开球策略
faceoff_defense  = 1                # 开球防守策略树
flag_faceoff_defense = -1           # 如果为1 则换开球防守策略

last_futuerballx = 0
last_futuerbally = 0
tickBeginPenalty = 0
tickBeginGoalKick = 0
tickBeginPlaceKick = 0
lastBallx = -110 + 37.5
lastBally = 0
BallPos = [Vector2(0, 0)] * 100000
resetHistoryRecord = False
newMatch = False

# 打印比赛状态，详细请对比v5rpc.py
@unbox_event
def on_event(event_type: int, args: EventArguments):
    event = {
        0: lambda: print(args.judge_result.reason),
        1: lambda: print("Match Start"),
        2: lambda: print("Match Stop"),
        3: lambda: print("First Half Start"),
        4: lambda: print("Second Half Start"),
        5: lambda: print("Overtime Start"),
        6: lambda: print("Penalty Shootout Start"),
        7: lambda: print("MatchShootOutStart"),
        8: lambda: print("MatchBlockStart")
    }
    global race_state_trigger
    global race_state
    if event_type == 0:
        race_state = args.judge_result.type
        race_state_trigger = args.judge_result.offensive_team
        if race_state == JudgeResultEvent.ResultType.PlaceKick:
            print("Place Kick")
        elif race_state == JudgeResultEvent.ResultType.PenaltyKick:
            print("Penalty Kick")
        elif race_state == JudgeResultEvent.ResultType.GoalKick:
            print("Goal Kick")
        elif (race_state == JudgeResultEvent.ResultType.FreeKickLeftBot
              or race_state == JudgeResultEvent.ResultType.FreeKickRightBot
              or race_state == JudgeResultEvent.ResultType.FreeKickLeftTop
              or race_state == JudgeResultEvent.ResultType.FreeKickRightTop):
            print("Free Kick")

        actor = {
            Team.Self: lambda: print("By Self"),
            Team.Opponent: lambda: print("By Opp"),
            Team.Nobody: lambda: print("By Nobody"),
        }
        actor[race_state_trigger]()

    event[event_type]()


@unbox_int
def get_team_info(server_version: int) -> str:
    version = {
        0: "V1.0",
        1: "V1.1"
    }
    print(f'server rpc version: {version.get(server_version, "V1.0")}')
    global newMatch
    newMatch = True
    return 'YL'             # 在此行修改双引号中的字符串为自己的队伍名

'守门员策略'
def goalkeeper (field):
    global football_now_y,football_now_x
    global futureBally,futureBallx
    global last_futuerballx
    # 预测代码
    football_now_x = field.ball.position.x
    football_now_y = field.ball.position.y
    futureBallx = 8 * football_now_x - 7 * BallPos[GlobalVariable.tick - 1].x
    futureBally = 8 * football_now_y - 7 * BallPos[GlobalVariable.tick - 1].y
    # 球到守门员的距离
    dx = football_now_x - baseRobots[0].get_pos().x
    dy = football_now_y - baseRobots[0].get_pos().y
    distance = math.sqrt(dx * dx + dy * dy)
    # 球到己方球门中心的距离
    dx2 = futureBallx - (-110)
    dy2 = futureBally
    distance_ball = math.sqrt(dx2 ** 2 + dy2 ** 2)
    # 己方大禁区内对方球员数量
    num_in_goal = 0
    for i in range(0, 5):
        if oppRobots[i].get_pos().x <= -75 and abs(oppRobots[i].get_pos().y) <= 40:  # 如果有
            num_in_goal = 1 + num_in_goal
    # 球在己方半场
    if football_now_x <= 5:
        if abs(futureBally) < 25:                       # 小禁区y范围内
            baseRobots[0].moveto(-105, futureBally)     # 守门员横向移动防守
            if futureBallx <= -95 :                     # 球在小禁区x范围内
                baseRobots[0].goalkeeper_throw_ball(football_now_x, football_now_y)  # 守门员移动到球的位置把球踢开
            if football_now_x <= -85 :                  # 球快到小禁区x范围
                if num_in_goal <= 1:                    # 对方大禁区内人不超过一人
                    baseRobots[0].moveto(futureBallx, futureBally) # 守门员移动到球的位置
                    if distance <= 4 and futureBallx >= baseRobots[0].get_pos().x: # 球在附近并且在守门员前方
                        if futureBally > 0:
                            baseRobots[0].set_wheel_velocity(-125, 125)   # 守门员向左把球踢开
                        else:
                            baseRobots[0].set_wheel_velocity(125, -125)   # 守门员向右把球踢开
        elif 25 <= abs(futureBally) <= 60:              # 球在x＞-85 并且y处于中间时
            if football_now_x >= -70 :                   # 右半区
                if futureBally > 0:  # 球在上半区
                    baseRobots[0].moveto(-106, 5)
                    baseRobots[0].goalkeeper_throw_ball(football_now_x, football_now_y)  # 守门员准备踢球
                else:
                    baseRobots[0].moveto(-106, -5)
                    baseRobots[0].goalkeeper_throw_ball(football_now_x, football_now_y)  # 守门员准备踢球
            else :                                      #左半区
                if futureBally > 0:  # 球在上半区
                    baseRobots[0].moveto(-106, 16)
                    baseRobots[0].goalkeeper_throw_ball(football_now_x, football_now_y)  # 守门员准备踢球
                else:
                    baseRobots[0].moveto(-106, -16)
                    baseRobots[0].goalkeeper_throw_ball(football_now_x, football_now_y)  # 守门员准备踢球
        elif abs(futureBally) > 60 :                    # 球在x＞-85 并且靠近边线时
            if  futureBallx <= -70 :                    # 球快到大禁区x范围时
                if futureBally > 0 :                    # 球在上半区
                    baseRobots[0].moveto(-105, 15)
                else :                                  # 球在下半区
                    baseRobots[0].moveto(-105, -15)
            elif futureBallx <= -95 :                   # 球在小禁区x范围内
                if futureBally > 0:                     # 球在上半区
                    if num_in_goal <= 0:                # 没有敌方球员
                        baseRobots[0].goalkeeper_throw_ball(football_now_x, football_now_y) # 把球踢开
                    else:                               #  有敌方球员，定点防守
                        baseRobots[0].moveto(-106, 18)
                else:                                   # 球在下半区
                    if num_in_goal <= 0:                # 没有敌方球员
                        baseRobots[0].goalkeeper_throw_ball(football_now_x, football_now_y) #  把球踢开
                    else:                               # 有敌方球员，定点防守
                        baseRobots[0].moveto(-106, -18)
            else :                                      # 球在大禁区范围外
                if futureBally > 0 :                    # 球在上半区
                    baseRobots[0].moveto(-105, 13)
                else :
                    baseRobots[0].moveto(-105, -13)     # 球在下半区
    # 在对方
    else:
        if abs(futureBally) <= 23:                      # 球在球门范围内
            baseRobots[0].moveto(-106, futureBally)     # 横向移动准备防守
        elif futureBally > 0:                           # 上半区
            baseRobots[0].moveto(-106, 10)
        elif futureBally < 0:                           # 下半区
            baseRobots[0].moveto(-106, -10)

'点球守门员策略'
def tick_goalkeeper (field):
    global football_now_y,football_now_x
    global futureBally,futureBallx
    global flag_tick, tick
    # 预测代码
    football_now_x = field.ball.position.x
    football_now_y = field.ball.position.y
    futureBallx = 8 * football_now_x - 7 * BallPos[GlobalVariable.tick - 1].x
    futureBally = 8 * football_now_y - 7 * BallPos[GlobalVariable.tick - 1].y

    # 球到守门员的距离
    dx = football_now_x - baseRobots[0].get_pos().x
    dy = football_now_y - baseRobots[0].get_pos().y
    distance = math.sqrt(dx * dx + dy * dy)

    if tick == 1:  #守门线
        if GlobalVariable.tick - tickBeginPenalty <= 50:
            flag_tick = 1  # 换策略
            baseRobots[0].keeper_move(-106, futureBally * 1.1) #直上直下
            if distance <= 4 and futureBallx >= baseRobots[0].get_pos().x:  # 球在附近并且在守门员前方
                if futureBally > 0:
                    baseRobots[0].set_wheel_velocity(-125, 125)  # 守门员向左把球踢开
                else:
                    baseRobots[0].set_wheel_velocity(125, -125)  # 守门员向右把球踢开
        else:
            flag_tick = -1  # 对方没进球，取消换策略
            strategy_common(field)  # 调用常规策略
    elif tick == 2:  # 直撞
        if GlobalVariable.tick - tickBeginPenalty <= 50:
            flag_tick = 1  # 换策略
            baseRobots[0].moveto( football_now_x , futureBally * 1.1)
            if distance <= 4 and futureBallx >= baseRobots[0].get_pos().x:  # 球在附近并且在守门员前方
                if futureBally > 0:
                    baseRobots[0].set_wheel_velocity(-125, 125)  # 守门员向左把球踢开
                else:
                    baseRobots[0].set_wheel_velocity(125, -125)  # 守门员向右把球踢开
        else:
            flag_tick = -1  # 对方没进球，取消换策略
            strategy_common(field)  # 调用常规策略



'常规策略'
def strategy_common(field):  # 正常的时候

    global football_now_x, football_now_y
    global futureBallx, futureBally
    # 预测足球位置
    football_now_x = field.ball.position.x
    football_now_y = field.ball.position.y
    # 预测足球位置
    global futureBallx, futureBally
    futureBallx = 8 * football_now_x - 7 * BallPos[GlobalVariable.tick - 1].x
    futureBally = 8 * football_now_y - 7 * BallPos[GlobalVariable.tick - 1].y

    baseRobots[1].moveto(-60, futureBally)
    baseRobots[2].move(35, -60)
    baseRobots[3].move(35, 60)
    baseRobots[4].move(30, 0)
    goalkeeper(field)

    dx1 = football_now_x - baseRobots[1].get_pos().x
    dy1 = football_now_y - baseRobots[1].get_pos().y
    distance1 = math.sqrt(dx1 * dx1 + dy1 * dy1)

    if football_now_x > 100 :
        baseRobots[1].moveto(-60, futureBally)
        baseRobots[2].move(70, -45)
        baseRobots[3].move(70, 45)
        baseRobots[4].move(60, 0)
        goalkeeper(field)
    elif football_now_x > 5 :
        if futureBallx  < 73 :
            baseRobots[4].shoot(futureBallx - 1, futureBally)
            baseRobots[3].move_with_angle(futureBallx - 1, futureBally, 0)
        else:
            distance1_ball = math.sqrt((football_now_x - baseRobots[4].get_pos().x) ** 2 + (
                    football_now_y - baseRobots[4].get_pos().y) ** 2)
            if futureBallx > 85 and distance1_ball < 7:
                if baseRobots[4].get_pos().x >= futureBallx:
                    baseRobots[4].breakthrough(futureBallx, futureBally, futureBallx - 3, futureBally)
                else:
                    if football_now_y > 0:
                        baseRobots[4].set_wheel_velocity(125, -125)
                    else:
                        baseRobots[4].set_wheel_velocity(-125, 125)
            else:
                # baseRobots[0].shoot (futureBallx+1,futureBally,10)
                baseRobots[4].shoot(futureBallx, futureBally)
                if (futureBallx > 90 and np.fabs(futureBally) <= 60):
                    baseRobots[4].moveto_dis(80, futureBally)

            baseRobots[3].shoot(futureBallx+18, futureBally)


        distance4_ball = math.sqrt(
            (football_now_x - baseRobots[2].get_pos().x) ** 2 + (football_now_y - baseRobots[2].get_pos().y) ** 2)
        if distance4_ball < 5 and (baseRobots[2].get_pos().x - football_now_x) < 0:  # 求到眼前了就中场射门
            if football_now_y > 0:
                baseRobots[2].set_wheel_velocity(125, -125)
            else:
                baseRobots[2].set_wheel_velocity(-125, 125)
        else:
            baseRobots[2].move_in_still_x(0, futureBally)

    elif football_now_x > -20 :
        baseRobots[2].move(35, -60)
        baseRobots[3].move(35, 60)
        baseRobots[4].move(30, 0)
        baseRobots[1].moveto(-60, futureBally * 1.1 )
        goalkeeper(field)
    elif football_now_x > -60 :
        baseRobots[2].move(35, -60)
        baseRobots[3].move(35, 60)
        baseRobots[4].move(30, 0)
        goalkeeper(field)
        if (distance1 >= 10 and futureBallx >= baseRobots[1].get_pos().x):
            baseRobots[1].moveto(futureBallx, futureBally )
        elif (distance1 <= 10 and futureBallx >= baseRobots[1].get_pos().x):
            if (futureBally < -30):
                baseRobots[1].set_wheel_velocity(-125, 125)
            elif (futureBally > 30):
                baseRobots[1].set_wheel_velocity(125, -125)
            else :
                baseRobots[1].moveto(futureBallx, futureBally)
    else :
        baseRobots[1].moveto(-60, -futureBally)
        baseRobots[2].move(35, -60)
        baseRobots[3].move(35, 60)
        baseRobots[4].move(30, 0)
        goalkeeper(field)




'点球策略'
def strategy_penalty(field):
    global tickBeginPenalty
    global race_state_trigger
    global flag_penalty,penalty
    # 预测球的位置
    football_now_x = field.ball.position.x
    football_now_y = field.ball.position.y
    futureBallx = 8 * football_now_x - 7 * BallPos[GlobalVariable.tick - 1].x
    futureBally = 8 * football_now_y - 7 * BallPos[GlobalVariable.tick - 1].y
    # 攻方策略
    if race_state_trigger == Team.Self:
        for i in range(0, 5):
            baseRobots[i].set_wheel_velocity(0, 0)
        if penalty == 1:#c字
            goalkeeper(field)               # 调用守门员策略
            if GlobalVariable.tick - tickBeginPenalty <= 15:
              baseRobots[1].set_wheel_velocity(125, 106)
            elif GlobalVariable.tick - tickBeginPenalty <= 19:
              baseRobots[1].set_wheel_velocity(125, 43)
            elif GlobalVariable.tick - tickBeginPenalty <= 30:
              baseRobots[1].set_wheel_velocity(125, -125)
            else:
                flag_penalty = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif penalty == 2:#直踢
            goalkeeper(field)  # 调用守门员策略
            if GlobalVariable.tick - tickBeginPenalty <= 30:
                if np.fabs(baseRobots[1].get_pos().x) < 64.9999:
                    baseRobots[1].set_wheel_velocity(125, 125)
                else:
                    baseRobots[1].set_wheel_velocity(125, -125)
            else:
                flag_penalty = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif penalty == 3:#c射
            goalkeeper(field)  # 调用守门员策略
            if GlobalVariable.tick - tickBeginPenalty <= 15:
                baseRobots[1].set_wheel_velocity(125, 107)
            elif GlobalVariable.tick - tickBeginPenalty <= 19:
                baseRobots[1].set_wheel_velocity(125, -125)
            else:
                flag_penalty = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif penalty == 4:#c甩
            goalkeeper(field)  # 调用守门员策略
            if GlobalVariable.tick - tickBeginPenalty <= 15:
                baseRobots[1].set_wheel_velocity(125, 107+0.05)
            elif GlobalVariable.tick - tickBeginPenalty <= 30:
                baseRobots[1].set_wheel_velocity(125, -125)
            else:
                flag_penalty = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
    # 守方策略
    if race_state_trigger == Team.Opponent:
        if GlobalVariable.tick - tickBeginPenalty <= 60 :
            baseRobots[1].moveto_dis(50, -15)
            baseRobots[2].moveto_dis(50, 15)
            baseRobots[3].moveto_dis(70, -65)
            baseRobots[4].moveto_dis(70, 65)
            tick_goalkeeper (field)              # 调用守门员策略
        else:
            strategy_common(field)          # 调用常规策略

'门球策略'
def strategy_goalkick(field):
    global tickBeginGoalKick
    global race_state_trigger
    global goal,flag_goal
    global guard_goal

    # 预测足球位置
    football_now_x = field.ball.position.x
    football_now_y = field.ball.position.y
    futureBallx = 8 * football_now_x - 7 * BallPos[GlobalVariable.tick - 1].x
    futureBally = 8 * football_now_y - 7 * BallPos[GlobalVariable.tick - 1].y
    # 球到2号球员的距离
    dx2 = futureBallx - baseRobots[2].get_pos().x
    dy2 = futureBally - baseRobots[2].get_pos().y
    distance2 = math.sqrt(dx2 * dx2 + dy2 * dy2)
    # 球到3号球员的距离
    dx3 = futureBallx - baseRobots[3].get_pos().x
    dy3 = futureBally - baseRobots[3].get_pos().y
    distance3 = math.sqrt(dx3 * dx3 + dy3 * dy3)
    # 球到一号球员的距离
    dx1 = futureBallx - baseRobots[1].get_pos().x
    dy1 = futureBally - baseRobots[1].get_pos().y
    distance1 = math.sqrt(dx1 * dx1 + dy1 * dy1)
    # 攻方策略
    if race_state_trigger == Team.Self:
        if goal == 1: #二传
            if GlobalVariable.tick - tickBeginGoalKick <= 48:
                for i in range(0, 5):
                    baseRobots[i].set_wheel_velocity(0, 0)
                baseRobots[0].set_wheel_velocity(125, 125)
                baseRobots[1].set_wheel_velocity(-125, 125)
                baseRobots[3].set_wheel_velocity(125, 125)
                baseRobots[4].set_wheel_velocity(125, -125)
            elif GlobalVariable.tick - tickBeginGoalKick <= 75:
                baseRobots[2].set_wheel_velocity(125, 125)
            else:
                flag_goal = 1
                strategy_common(field)

        elif goal == 2:  #陕师大
            if GlobalVariable.tick - tickBeginGoalKick <= 75:
                for i in range(0, 5):
                    baseRobots[i].set_wheel_velocity(0, 0)
                baseRobots[0].set_wheel_velocity(125, 125)
            else:
                flag_goal = 1               # 球没进换策略
                strategy_common(field)      # 调用常规策略
    # 守方策略
    if race_state_trigger == Team.Opponent:
        if guard_goal == 1:                 # 1号横向移动，其他追球
            goalkeeper(field)
            if GlobalVariable.tick - tickBeginGoalKick <= 88:
                if    -70 < futureBallx < 75:
                    baseRobots[1].moveto(-55, futureBally)
                    baseRobots[2].moveto(futureBallx, futureBally)
                    baseRobots[3].moveto(futureBallx, futureBally)
                    baseRobots[4].moveto(futureBallx, futureBally)
                else:
                    baseRobots[1].moveto(-55, futureBally)
                    baseRobots[2].set_wheel_velocity(0, 0)
                    baseRobots[3].set_wheel_velocity(0, 0)
                    baseRobots[4].set_wheel_velocity(0, 0)
            else:
                strategy_common(field)      # 调用常规策略
        elif guard_goal == 2:               # 防止被造犯规
            goalkeeper(field)               # 调用守门员策略
            baseRobots[1].moveto(-20 , futureBally)
            baseRobots[2].moveto(-40 , futureBally)
            baseRobots[3].moveto(-55 , futureBally)
            baseRobots[4].moveto(-70 , futureBally)


'开球策略'
def strategy_PlaceKick (field): #开球策略             开球改动地方
    global tickBeginPlaceKick
    global race_state_trigger
    global faceoff
    global flag_faceoff
    global flag_faceoff_defense, faceoff_defense
    # 预测足球位置
    football_now_x = field.ball.position.x
    football_now_y = field.ball.position.y
    futureBallx = 8 * football_now_x - 7 * BallPos[GlobalVariable.tick - 1].x
    futureBally = 8 * football_now_y - 7 * BallPos[GlobalVariable.tick - 1].y
    # 2号球员到球的距离
    dx2 = futureBallx - baseRobots[2].get_pos().x
    dy2 = futureBally - baseRobots[2].get_pos().y
    distance2 = math.sqrt(dx2 * dx2 + dy2 * dy2)
    # 3号球员到球的距离
    dx3 = futureBallx - baseRobots[3].get_pos().x
    dy3 = futureBally - baseRobots[3].get_pos().y
    distance3 = math.sqrt(dx3 * dx3 + dy3 * dy3)
    # 1号球员到球的距离
    dx1 = futureBallx - baseRobots[1].get_pos().x
    dy1 = futureBally - baseRobots[1].get_pos().y
    distance1 = math.sqrt(dx1 * dx1 + dy1 * dy1)
    # 攻方策略
    if race_state_trigger == Team.Self:
        if faceoff == 1:                    # 向己方踢反弹
            for i in range(2, 5):
                baseRobots[i].set_wheel_velocity(0, 0)
            if GlobalVariable.tick - tickBeginPlaceKick <= 95:
                baseRobots[1].set_wheel_velocity(125, 125)
                goalkeeper(field)
            else :
                flag_faceoff = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif  faceoff == 2:                 # 3、4配合弧线切进
            for i in range(1, 5):
                baseRobots[i].set_wheel_velocity(0, 0)
            goalkeeper(field)
            if GlobalVariable.tick - tickBeginPlaceKick <= 30:
                baseRobots[4].set_wheel_velocity(90, 90)
                baseRobots[3].set_wheel_velocity(125, 125)
            elif GlobalVariable.tick - tickBeginPlaceKick <= 32:
                baseRobots[3].set_wheel_velocity(125, 125)
                baseRobots[4].set_wheel_velocity(125, 100)
            elif GlobalVariable.tick - tickBeginPlaceKick <= 80:
                baseRobots[3].set_wheel_velocity(-125, 125)
                baseRobots[4].set_wheel_velocity(125, -125)
            else:
                flag_faceoff = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif faceoff == 3 :                 # 1、2号夹击
            for i in range(3 , 5):
                baseRobots[i].set_wheel_velocity(0, 0)
            if GlobalVariable.tick - tickBeginPlaceKick <= 95:
                baseRobots[1].set_wheel_velocity(125, 125)
                baseRobots[2].set_wheel_velocity(125, 125)
                goalkeeper(field)
            else :
                flag_faceoff = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif faceoff == 4:
            for i in range(1, 5):
                baseRobots[i].set_wheel_velocity(0, 0)
            goalkeeper(field)
            if GlobalVariable.tick - tickBeginPlaceKick <= 34:
                baseRobots[4].set_wheel_velocity(90, 90)
                baseRobots[3].set_wheel_velocity(125, 125)
            elif GlobalVariable.tick - tickBeginPlaceKick <= 80:
                baseRobots[3].set_wheel_velocity(-125, 125)
                baseRobots[4].set_wheel_velocity(-125, 125)
            else:
                flag_faceoff = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif faceoff == 5:
            for i in range(1, 5):
                baseRobots[i].set_wheel_velocity(0, 0)
            goalkeeper(field)
            if GlobalVariable.tick - tickBeginPlaceKick <= 31.8:
                baseRobots[4].set_wheel_velocity(90, 90)
                baseRobots[3].set_wheel_velocity(125, 125)
            elif GlobalVariable.tick - tickBeginPlaceKick <= 34:
                baseRobots[4].set_wheel_velocity(90, 90)
                baseRobots[3].set_wheel_velocity(-125, 125)
            elif GlobalVariable.tick - tickBeginPlaceKick <= 65:
                baseRobots[3].set_wheel_velocity(-125, 125)
                baseRobots[4].set_wheel_velocity(-125, 125)
            else:
                flag_faceoff = 1            # 球没进换策略
                strategy_common(field)      # 调用常规策略
        elif faceoff == 6:
            if race_state_trigger == Team.Self:
                for i in range(1, 5):
                    baseRobots[i].set_wheel_velocity(0, 0)
                goalkeeper(field)
                if GlobalVariable.tick - tickBeginPlaceKick <= 28:
                    baseRobots[4].set_wheel_velocity(125, 125)
                    baseRobots[3].set_wheel_velocity(100, 100)
                elif GlobalVariable.tick - tickBeginPlaceKick <= 42:
                    baseRobots[3].set_wheel_velocity(125, 100)
                    baseRobots[4].set_wheel_velocity(0, 0)
                elif GlobalVariable.tick - tickBeginPlaceKick <= 70:
                    baseRobots[3].set_wheel_velocity(-125, 125)
                    baseRobots[4].set_wheel_velocity(0, 0)
                else:
                    flag_faceoff = 1        # 球没进换策略
                    strategy_common(field)  # 调用常规策略

    # 守方
    if race_state_trigger == Team.Opponent:
        if faceoff_defense == 1 :                       # 一动三静型防守
            if GlobalVariable.tick - tickBeginPlaceKick <= 100:
                baseRobots[4].shoot(futureBallx, futureBally)
                flag_faceoff_defense = 1                # 准备换策略
                goalkeeper(field)
                if (futureBallx < -35 and np.fabs(futureBally) < 40) or (
                        football_now_x < -33 and np.fabs(football_now_y) < 40):
                    baseRobots[3].set_wheel_velocity(0, 0)
                    if distance3 <= 10 and futureBallx >= baseRobots[3].get_pos().x:
                        baseRobots[3].set_wheel_velocity(125, -125)
                    baseRobots[2].set_wheel_velocity(0, 0)
                    if distance2 <= 10 and futureBallx >= baseRobots[2].get_pos().x:
                        baseRobots[2].set_wheel_velocity(-125, 125)
                    baseRobots[1].set_wheel_velocity(0, 0)
                    if distance1 <= 10 and futureBallx >= baseRobots[1].get_pos().x:
                        baseRobots[1].set_wheel_velocity(-125, 125)
                else:
                    baseRobots[1].set_wheel_velocity(0, 0)
                    baseRobots[2].set_wheel_velocity(0, 0)
                    baseRobots[3].set_wheel_velocity(0, 0)
            else:
                flag_faceoff_defense = -1               # 对方没进球，取消换策略
                strategy_common(field)                  # 调用常规策略
        elif faceoff_defense == 2:
            if GlobalVariable.tick - tickBeginPlaceKick <= 100:
                flag_faceoff_defense = 1  # 准备换策略
                goalkeeper(field)
                baseRobots[1].set_wheel_velocity(0, 0)
                baseRobots[2].set_wheel_velocity(0, 0)
                baseRobots[3].set_wheel_velocity(0, 0)
                baseRobots[4].set_wheel_velocity(0, 0)
            else:
                flag_faceoff_defense = -1  # 对方没进球，取消换策略
                strategy_common(field)
        elif faceoff_defense == 3:  # 四人静止型开球防守
            if GlobalVariable.tick - tickBeginPlaceKick <= 100:
                flag_faceoff_defense = 1  # 准备换策略
                goalkeeper(field)
                baseRobots[1].set_wheel_velocity(0, 0)
                baseRobots[2].set_wheel_velocity(0, 0)
                baseRobots[3].set_wheel_velocity(0, 0)
                baseRobots[4].set_wheel_velocity(0, 0)
            else:
                flag_faceoff_defense = -1  # 对方没进球，取消换策略
                strategy_common(field)




@unbox_field
def get_instruction(field: Field):  #策略接口
    # python start.py 20000    print(field.tick)  # tick从2起始
    GlobalVariable.tick = field.tick
    global resetHistoryRecord

    for i in range(0, 5): # 0 1 2 3 4
        baseRobots.append(BaseRobot())
        oppRobots.append(BaseRobot())
        baseRobots[i].update(field.self_robots[i], resetHistoryRecord)
        oppRobots[i].update(field.opponent_robots[i], resetHistoryRecord)
        global newMatch
        if field.tick == 2: #newMatch is True:
            for j in range(0, 8):
                baseRobots[i].HistoryInformation[j] = field.self_robots[i].copy()   # 第0拍主动维护历史数据
                baseRobots[i].PredictInformation[j] = field.self_robots[i].copy()	# 第0拍主动维护预测数据
            newMatch = False
        baseRobots[i].PredictRobotInformation(GlobalVariable.tick_delay)#(GlobalVariable.tick_delay)

    football_now_x = -field.ball.position.x   # 黄方假设，球坐标取反
    football_now_y = -field.ball.position.y

    field.ball.position.x = -field.ball.position.x
    field.ball.position.y = -field.ball.position.y

    global BallPos
    BallPos[GlobalVariable.tick] = Vector2(football_now_x, football_now_y)
    if resetHistoryRecord is True:
        for i in range(GlobalVariable.tick, GlobalVariable.tick - 11, -1):
            BallPos[i] = Vector2(football_now_x, football_now_y)
    #print (race_state)
    #根据情况执行什么样的策略
    if race_state == JudgeResultEvent.ResultType.PenaltyKick:
        strategy_penalty(field)  #点球策略 2
    elif race_state == JudgeResultEvent.ResultType.GoalKick:
        strategy_goalkick(field) #门球策略 1
    elif race_state == JudgeResultEvent.ResultType.PlaceKick:
        strategy_PlaceKick(field) #开球策略0
    else :
        strategy_common(field)


    for i in range(0, 5):
        baseRobots[i].save_last_information(football_now_x, football_now_y)
    data_loader.set_tick_state(GlobalVariable.tick, race_state)
    resetHistoryRecord = False

    velocity_to_set = []
    for i in range(0, 5):
        velocity_to_set.append((baseRobots[i].robot.wheel.left_speed, baseRobots[i].robot.wheel.right_speed))

    return velocity_to_set, 0    # 以第二元素的(0,1)表明重置开关,1表示重置


@unbox_field
def get_placement(field: Field) -> List[Tuple[float, float, float]]:  #返回摆球时的坐标 接口
    final_set_pos: List[Union[Tuple[int, int, int], Tuple[float, float, float]]]
    global resetHistoryRecord
    resetHistoryRecord = True
    global flag_penalty,penalty
    global flag_goal,goal
    global flag_tick, tick
    global last_race_state,last_race_state_trigger
    global guard_goal
    global defend_flag,time
    global goal_time
    global faceoff
    global flag_faceoff
    global flag_faceoff_defense , faceoff_defense
    defend_flag = 0

    '点球决策树'
    if  flag_penalty == 1 :
        penalty = penalty + 1
        if penalty > 4:
            penalty = 1
    flag_penalty = -1

    '门球决策树'
    if flag_goal == 1:
        goal = goal + 1
        if goal > 2:
            goal = 1
    flag_goal = -1

    '门球防止被造犯规'
    if (last_race_state == 1 and race_state == 1) and (last_race_state_trigger != Team.Self and race_state != Team.Self):
            guard_goal = 2
    else :
            guard_goal = 1

    '开球决策树'
    if flag_faceoff == 1:
        faceoff  = faceoff + 1
        if faceoff > 6:
            faceoff = 1
    flag_faceoff = -1

    '开球防守决策树'
    if flag_faceoff_defense == 1:
        faceoff_defense  = faceoff_defense + 1
        if faceoff_defense > 3:
            faceoff_defense = 1
    flag_faceoff_defense = -1

    '点球防守策略树'
    if flag_tick == 1:
        tick  = tick + 1
        if tick > 2:
            tick = 1
    flag_tick = -1


    if race_state == JudgeResultEvent.ResultType.PlaceKick:
        global tickBeginPlaceKick
        tickBeginPlaceKick = field.tick
        if race_state_trigger == Team.Self:
            print("开球进攻摆位")
            if faceoff == 1 :
                set_pos = [[GlobalVariable.goalkepper_X, 0, 90],
                           [13, -6.2, 155],
                           [-5, -60, 0],
                           [-50, 0, 0],
                           [-44, 83, 152],
                           [0.0, 0.0, 0.0]]
            elif faceoff == 2 :
                set_pos = [[GlobalVariable.goalkepper_X, 0, 90],
                           [-55, 40, 90],
                           [-5, -60, 0],
                           [-25, 24, 0],
                           [-20, -6, 20],
                           [-5, 0.0, 0.0]]
            elif faceoff == 3 :
                set_pos = [[GlobalVariable.goalkepper_X, 0, 90],
                           [9, -9, 135],
                           [-57, 56, -49.5],
                           [-50, 0, 0],
                           [-44, 83, 152],
                           [-5, 0.0, 0.0]]
            elif faceoff == 4 :
                set_pos = [[GlobalVariable.goalkepper_X, 0, 90],
                           [-55, 40, 90],
                           [-5, -60, 0],
                           [-25, 24, 0],
                           [-9, -1, 6.34],
                           [-5, 0.0, 0.0]]
            elif faceoff == 5 :
                set_pos = [[GlobalVariable.goalkepper_X, 0, 90],
                           [-55, 40, 90],
                           [-5, -60, 0],
                           [-6, 45, -21.48],
                           [-9, -1, 6.34],
                           [-5, 0.0, 0.0]]
            elif faceoff == 6 :
                set_pos = [[GlobalVariable.goalkepper_X, 0, 90],
                           [-55, 40, 90],
                           [-5, -60, 0],
                           [-5, 58, 0],
                           [-4, -3, 36.87],
                           [-5, 0.0, 0.0]]

            # set_pos = [(-103, 0, 90), (30, 0, 0), (-3, -10, 0), (-3, 10, 0), (-3, 0, 0), (0.0, 0.0, 0.0)]
        else:   # if race_state_trigger == Team.Opponent:
            print("开球防守摆位")
            if faceoff_defense == 1 :   # 一动三静
                set_pos = [
                    [-108, -5, 0],
                    [-65, 4, 0],
                    [-60, -20, 0],
                    [-60, 20, 0],
                    [-5, 0, 0],
                    [0.0, 0.0, 0.0]]
            elif faceoff_defense == 2 :   # 一动三静
                set_pos = [
                    [-110, 0, 0],
                    [-71, 5.8, 0],
                    [-71, -5.8, 0],
                    [-71, -16.6, 0],
                    [-71, 16.6, 0],
                    [-5, 0.0, 0.0]]
            elif faceoff_defense == 3 :  # 四静
                set_pos = [
                    [-110, 0, 0],
                    [-71, 38.2, 0],
                    [-71, -38.2, 0],
                    [-71, -27.4, 0],
                    [-71, 27.4, 0],
                    [-5, 0.0, 0.0]]

            # set_pos = [(-105, 0, 90), (10, 20, -90), (10, -20, -90), (10, 40, -90), (10, -40, -90), (0.0, 0.0, 0.0)]
    elif race_state == JudgeResultEvent.ResultType.PenaltyKick:
        global tickBeginPenalty
        tickBeginPenalty = field.tick
        if race_state_trigger == Team.Self:
            print("点球进攻摆位")
            if penalty == 1:
               set_pos = [[-103, 0, 0],
                          [68.7, -10, 110],
                          [-50, -50, -30],
                          [-10, 50, -25],
                          [-30, 0, 0],
                          [5, 10, 0.0]]
            elif penalty == 2:
                set_pos = [[-103, 0, 0],
                          [65-2, 1+0.728,-20],
                          [-50, -50, -30],
                          [-10, 50, -25],
                          [-30, 0, 0],
                          [5, 10, 0.0]]
            elif penalty == 3:
               set_pos = [[-103, 0, 0],
                          [68.7, -10, 110],
                          [-50, -50, -30],
                          [-10, 50, -25],
                          [-30, 0, 0],
                          [5, 10, 0.0]]
            elif penalty == 4:
               set_pos = [[-103, 0, 0],
                          [68.7, -10, 110],
                          [-50, -50, -30],
                          [-10, 50, -25],
                          [-30, 0, 0],
                          [5, 10, 0.0]]
        else:   # if race_state_trigger == Team.Opponent:
            defend_flag = 1
            print("点球防守摆位")
            set_pos = [[-106, -10, 90],
                       [5, -50, 140],
                       [5, -60, -140],
                       [5, 85, -165],
                       [5, 61, 165],
                       [0, 0.0, 0.0]]
    elif race_state == JudgeResultEvent.ResultType.GoalKick:
        global tickBeginGoalKick
        tickBeginGoalKick = field.tick
        if race_state_trigger == Team.Self:
            print("门球进攻摆位")
            if goal == 1:
                set_pos = [[-110, 1, -47],
                          [-51, -10, -90],
                          [-10, -80, 45],
                          [-63, -60, 130.5],
                          [-25, -78, -90],
                          [-105, -5, 0.0]]
            elif goal == 2:
                set_pos = [[-102, -5.05, -90],
                           [-50, -20, -90],
                           [-50, -40, -90],
                           [-30, 0, -90],
                           [-50, 0, -90],
                           [-104.5, -9.05, 0.0]]
        else:   # if race_state_trigger == Team.Opponent:
            print("门球防守摆位")
            set_pos = [[-108, 0, -90],
                       [-68, -30, 0],
                       [-30, -45, 0],
                       [-30, 45, 0],
                       [-68, 30, 0],
                       [0.0, 0.0, 0.0]]
    elif (race_state == JudgeResultEvent.ResultType.FreeKickLeftTop
          or race_state == JudgeResultEvent.ResultType.FreeKickRightTop
          or race_state == JudgeResultEvent.ResultType.FreeKickRightBot
          or race_state == JudgeResultEvent.ResultType.FreeKickLeftBot):
        if race_state_trigger == Team.Self:
            print("争球进攻摆位")
            set_pos = [[-103, 0, 90],
                       [30, 0, 0],
                       [-3, -10, 0],
                       [-3, 10, 0],
                       [-3, 0, 0],
                       [0.0, 0.0, 0.0]]
        else:   # if race_state_trigger == Team.Opponent:
            print("争球防守摆位")
            set_pos = [[-105, 0, 0],
                       [30, 0, 0],
                       [10, -10, 0],
                       [10, 10, 0],
                       [10, 0, 0],
                       [0.0, 0.0, 0.0]]
    else:
        print("race_state = " + str(race_state))

    for set_pos_s in set_pos:  # 摆位反转
        set_pos_s[0] = -set_pos_s[0]
        set_pos_s[1] = -set_pos_s[1]
        set_pos_s[2] -= 180
        if set_pos_s[2] < -180:
            set_pos_s[2] += 360
    final_set_pos = [(set_pos[0][0], set_pos[0][1], set_pos[0][2]),
                     (set_pos[1][0], set_pos[1][1], set_pos[1][2]),
                     (set_pos[2][0], set_pos[2][1], set_pos[2][2]),
                     (set_pos[3][0], set_pos[3][1], set_pos[3][2]),
                     (set_pos[4][0], set_pos[4][1], set_pos[4][2]),
                     (set_pos[5][0], set_pos[5][1], set_pos[5][2])]

    last_race_state = race_state   #记录上一次比赛状态
    last_race_state_trigger  = race_state_trigger

    print(final_set_pos)
    return final_set_pos  # 最后一个是球位置（x,y,角）,角其实没用
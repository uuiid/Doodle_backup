# maya工具

在导出过程中关闭软件或者强制结束任务会产生不可预置的问题

- 需要 有 UE4 后缀的组, 使用这个组来输出abc文件
- 需要 解算从950开始
- 需要帧率时25fps
- 最好有一台摄像机用来创建拍屏
- 最好具有标记元数据, 使用这个元数据来确认解算的人物
- 导出fbx子项
- 导出fbx时是按照保存是引用保存时的状态加载的，
- 如果勾选直接加载所有引用的话， 会不查看保存状态， 直接加载所有引用

# 使用maya相同的骨骼作为父子约束

``` python
import maya.mel
l_select = cmds.ls(sl=True)

for p,s in zip(cmds.listRelatives(l_select[0],allDescendents=True,fullPath=True),cmds.listRelatives(l_select[1],allDescendents=True,fullPath=True)):
cmds.select([p,s])
maya.mel.eval(r'doCreateParentConstraintArgList 1 { "1","0","0","0","0","0","0","0","1","","1" };');
```
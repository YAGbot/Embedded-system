首先，下载安装git

1⃣️下载git： http://git-scm.com/downloads

2⃣️下载之后打开，双击.pkg安装

3⃣️打开终端，使用git --version命令查看安装版本，有就是安装成功了

4⃣️创建一个用户名：git config --global user.name "NAME" ， 这里的NAME可以随便取

5⃣️创建一个邮箱：git config --global user.email "MY_NAME@example.com" ，这里的邮箱应该跟gitblit服务器上注册的邮箱一致

然后，使用git命令上传本地文件至git服务器

1⃣️在终端进入你想要创建git的目录下，先初始化：git init

2⃣️连接远程服务器：git remote add origin ssh://xxxxx@10.2.10.208:29418/qinghua.git(这里写版本库地址)

3⃣️在将本地文件上传至git服务器之前，先上传一个测试文件，相当于激活一下服务器上的git库

4⃣️新建一个文件：touch README.md，这里的文件名可以随意取，创建完了之后可以用git status 查看一下

5⃣️将文件上传到本地git库：git add filename

6⃣️上传之前，给本次上传取一个名字：git commit －m  test ，这里的名字test可以随意取

7⃣️将本地git库的文件上传到git服务器库里：git push －u origin master
8⃣️如果报错：

error: failed to push some refs to 'ssh://rensainan@10.2.10.208:29418/qinghua.git'
hint: Updates were rejected because the remote contains work that you do
hint: not have locally. This is usually caused by another repository pushing
hint: to the same ref. You may want to first integrate the remote changes
hint: (e.g., 'git pull ...') before pushing again.
hint: See the 'Note about fast-forwards' in 'git push --help' for details.

那么则将上传的代码改为：git push －f origin master 即可

如果报错：

    fatal: 'origin' does not appear to be a git repository
    fatal: Could not read from remote repository.

那么输入git remote －v 查看自己的连接有没有问题，如果正常，则应显示：

origin	ssh://rensainan@10.2.10.208:29418/qinghua.git (fetch)
origin	ssh://rensainan@10.2.10.208:29418/qinghua.git (push)

如果什么都没有，则再输入一次步骤2⃣️的命令，继续查看

9⃣️完成测试文件的传输之后，重复步骤5⃣️6⃣️7⃣️8⃣️即可传输自己本地的文件。删除git，进入想要删除的目录，输入命令：rm -rf .git



Q:
error: failed to push some refs to 'ssh://yinaiguo@111.30.93.211:29418/Embedded-system.git'
hint: Updates were rejected because the remote contains work that you do
hint: not have locally. This is usually caused by another repository pushing
hint: to the same ref. You may want to first integrate the remote changes
hint: (e.g., 'git pull ...') before pushing again.
hint: See the 'Note about fast-forwards' in 'git push --help' for details.

解决方案

这个问题是因为远程库与本地库不一致造成的，那么我们把远程库同步到本地库就可以了。 
使用指令

git pull --rebase origin master


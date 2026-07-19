# MyNotes

## 1 git地址

https://github.com/Kunzite1/MyNotes.git

## 2 公司电脑做笔记

### 2-1 使用稀疏检出克隆文件夹

```powershell
# 1. 新建一个本地目录并进入
mkdir MyNotes-Office
cd MyNotes-Office

# 2. 初始化本地 Git 仓库
git init

# 3. 添加远程仓库地址
git remote add origin https://github.com/Kunzite1/MyNotes.git

# 4. 启用稀疏检出功能
git sparse-checkout init --cone

# 5. 设置想拉取的文件夹名称（可以写多个，空格隔开）
git sparse-checkout set CompanyNotes

# 6. 创建并切换到你在公司专属的工作分支（例如：office-work）
git checkout -b office-work

# 7 从远程的 main 分支拉取你指定的文件夹内容
git pull origin main
```

### 2-2 创建分支

### 2-3 完成后在公司电脑提交和推送和合并

用VScode或github desktop进行commit

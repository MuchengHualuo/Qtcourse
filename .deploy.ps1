# ============================================================
#  QtCourse -> GitHub 部署 / 推送脚本 (v3)
#
#  修正了两个真实问题：
#    A) git 调用 ssh 时没走 ~/.ssh/config 的 443 配置（实测 git 去连了 22 端口）
#       -> 改为把端口/密钥直接写进 git 自己的 core.sshCommand，不再依赖 config
#    B) v2 里 Copy-Item 失败了却仍打印成功
#       -> 现在检查 $? 并把结论放进变量
#
#  用法：
#     powershell -ExecutionPolicy Bypass -File D:\Qt\QtCourse\.deploy.ps1 -Push
#     powershell -ExecutionPolicy Bypass -File D:\Qt\QtCourse\.deploy.ps1 -Diag
# ============================================================
param(
    [switch]$Push,
    [switch]$Diag,
    [string]$UserName = "MuchengHualuo",
    [string]$Email    = "muchenghualuo@users.noreply.github.com",
    [string]$Remote   = "git@github.com:MuchengHualuo/Qtcourse.git"
)

$ErrorActionPreference = "Continue"
$RepoDir  = "D:\Qt\QtCourse"
$SshDir   = Join-Path $env:USERPROFILE ".ssh"
$KeyPath  = Join-Path $SshDir "id_ed25519_github"
$Expected = "SHA256:EtEapCmgaBUGGSoszmVSI8gIH1dQ0z9Yi0NVQpqIEl0"

function Say($m)  { Write-Host "  $m" }
function Head($m) { Write-Host "`n=== $m ===" -ForegroundColor Cyan }
function Ok($m)   { Write-Host "  [OK] $m" -ForegroundColor Green }
function Bad($m)  { Write-Host "  [!!] $m" -ForegroundColor Red }

# ---------- 诊断 ----------
Head "Diagnose"

# 1. git 版本
$gitExe = (Get-Command git -ErrorAction SilentlyContinue).Source
if (-not $gitExe) { Bad "git not found in PATH"; exit 1 }
Ok "git: $(& git --version)  ($gitExe)"

# 2. 密钥
if (-not (Test-Path $KeyPath)) {
    Bad "private key missing: $KeyPath"
    exit 1
}
$fp = (& ssh-keygen -lf $KeyPath 2>&1) -join ""
Say "local key : $fp"
if ($fp -match [regex]::Escape(($Expected -split ' ')[1])) { Ok "fingerprint matches expected" }
else { Bad "fingerprint mismatch, expected $Expected" }

# 3. SSH over 443（显式参数，不依赖 config）
Say "testing ssh over 443 ..."
$sshArgs = @("-p","443","-i",$KeyPath,
             "-o","IdentitiesOnly=yes",
             "-o","BatchMode=yes",
             "-o","StrictHostKeyChecking=accept-new",
             "-o","ConnectTimeout=30",
             "-T","git@ssh.github.com")
$r = (& ssh @sshArgs 2>&1) -join "`n"
Say $r
if ($r -match "successfully authenticated") {
    Ok "GitHub recognized this key"
    $script:sshOk = $true
} else {
    $script:sshOk = $false
    Bad "GitHub did NOT authenticate this key (see message above)"
}

if ($Diag -and -not $Push) { exit 0 }

# ---------- 推送 ----------
if ($Push) {
    Head "Commit and push"
    Set-Location $RepoDir

    # 关键修复：把 ssh 参数写进 git 自己的配置，绕开 ~/.ssh/config
    $sshCmd = "ssh -p 443 -i `"$KeyPath`" -o IdentitiesOnly=yes -o StrictHostKeyChecking=accept-new"
    & git config core.sshCommand $sshCmd
    Ok "core.sshCommand = $sshCmd"

    & git config user.name  $UserName
    & git config user.email $Email

    if (-not (Test-Path (Join-Path $RepoDir ".git"))) {
        & git init
    } else {
        Say "existing .git reused"
    }

    & git add -A
    Say "--- staged ---"
    & git status --short

    # 注意：git branch -M 必须放在首次 commit 之后，
    # 否则新仓库还没有 HEAD，会报 "not a valid object name: main"
    if (-not (& git rev-parse --verify HEAD 2>$null)) {
        & git commit -m "Qt sample 2.4: hybrid UI design with About tool button"
    } else {
        & git commit -m "Update Qt sample 2.4" 2>$null
    }

    & git branch -M main
    Ok "current branch = main"

    $remotes = & git remote
    if ($remotes -notcontains "origin") { & git remote add origin $Remote }
    else { & git remote set-url origin $Remote }
    Ok "remote = $Remote"

    Say "ls-remote (reachability) ..."
    & git ls-remote --heads origin 2>&1 | Select-Object -First 3

    Say "pushing ..."
    & git push -u origin main 2>&1

    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n  PUSH OK -> https://github.com/$UserName/Qtcourse" -ForegroundColor Green
    } else {
        Write-Host "`n  PUSH FAILED (see above)" -ForegroundColor Red
        Write-Host "  - 'Repository not found'      -> wrong user/repo name" -ForegroundColor Yellow
        Write-Host "  - 'Permission denied'         -> key not recognized by GitHub" -ForegroundColor Yellow
        Write-Host "  - 'Connection refused/refused'-> port 443 not taking effect" -ForegroundColor Yellow
    }
}

if (-not $Push -and -not $Diag) {
    Write-Host "Nothing to do. Use -Diag and/or -Push." -ForegroundColor Yellow
}

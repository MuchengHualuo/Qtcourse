# ============================================================
#  QtCourse -> GitHub 一键部署 / 推送脚本
#
#  用法（新开一个 PowerShell 窗口）：
#     powershell -ExecutionPolicy Bypass -File D:\Qt\QtCourse\.deploy.ps1 -Deploy -Push
#
#  只测认证不推送：
#     powershell -ExecutionPolicy Bypass -File D:\Qt\QtCourse\.deploy.ps1 -Test
# ============================================================
param(
    [switch]$Deploy,
    [switch]$Push,
    [switch]$Test,
    [string]$UserName = "MuchengHualuo",
    [string]$Email    = "muchenghualuo@users.noreply.github.com",
    [string]$Remote   = "git@github.com:MuchengHualuo/Qtcourse.git"
)

$ErrorActionPreference = "Continue"
$RepoDir   = "D:\Qt\QtCourse"
$KeyDir    = "D:\Qt\.git-setup"
$SshDir    = Join-Path $env:USERPROFILE ".ssh"
$Expected  = "SHA256:EtEapCmgaBUGGSoszmVSI8gIH1dQ0z9Yi0NVQpqIEl0"

function Say($m) { Write-Host "  $m" }
function Head($m) { Write-Host "`n=== $m ===" -ForegroundColor Cyan }

# ---------------- 1) 部署密钥与 SSH 配置 ----------------
if ($Deploy) {
    Head "[1/3] Deploy SSH key + config"

    if (-not (Test-Path $SshDir)) { New-Item -ItemType Directory -Path $SshDir -Force | Out-Null }

    Copy-Item (Join-Path $KeyDir "id_ed25519_github")     $SshDir -Force
    Copy-Item (Join-Path $KeyDir "id_ed25519_github.pub") $SshDir -Force
    Say "key copied to $SshDir"

    $cfg = Join-Path $SshDir "config"
    $existing = if (Test-Path $cfg) { Get-Content $cfg -Raw } else { "" }
    if ($existing -match "ssh\.github\.com") {
        Say "github.com block already in config"
    } else {
        $block = @"

# ==== GitHub (SSH over 443, required on this network) ====
Host github.com
    Hostname ssh.github.com
    Port 443
    User git
    IdentityFile ~/.ssh/id_ed25519_github
    IdentitiesOnly yes
"@
        Add-Content -LiteralPath $cfg -Value $block -Encoding UTF8
        Say "github block appended to config"
    }
}

# ---------------- 2) 诊断 SSH 认证 ----------------
if ($Deploy -or $Test) {
    Head "[2/3] Test GitHub SSH authentication"

    $fp = (& ssh-keygen -lf (Join-Path $SshDir "id_ed25519_github") 2>&1) -join ""
    Say "local key fingerprint : $fp"
    Say "expected fingerprint  : $Expected"
    if ($fp -notmatch [regex]::Escape(($Expected -split ' ')[1])) {
        Write-Host "  !! local key does NOT match the expected key" -ForegroundColor Red
    }

    Say "contacting github.com ..."
    $t = (& ssh -T -o BatchMode=yes -o ConnectTimeout=30 git@github.com 2>&1) -join "`n"
    Say $t

    if ($t -match "successfully authenticated") {
        Write-Host "  ==> SSH OK, GitHub recognized this key" -ForegroundColor Green
    } else {
        Write-Host "  ==> SSH FAILED: GitHub did not recognize this key." -ForegroundColor Red
        Write-Host ""
        Write-Host "  Add EXACTLY this line at https://github.com/settings/keys" -ForegroundColor Yellow
        Write-Host "  (tick Key type = Authentication Key; do NOT add it as a Deploy key)" -ForegroundColor Yellow
        Write-Host ""
        Write-Host ("  " + (Get-Content (Join-Path $SshDir "id_ed25519_github.pub") -Raw).Trim()) -ForegroundColor White
        Write-Host ""
        Write-Host "  After adding, GitHub shows a fingerprint. It MUST be:" -ForegroundColor Yellow
        Write-Host "  $Expected" -ForegroundColor White
        Write-Host ""
        Write-Host "  Note: 'ssh -T' failing is EXPECTED if the key was added as a" -ForegroundColor Gray
        Write-Host "  Deploy key (repo-level) instead of an account-level key." -ForegroundColor Gray
        Write-Host "  Deploy keys cannot authenticate the account, but they CAN" -ForegroundColor Gray
        Write-Host "  still push. So if ls-remote below succeeds, just continue." -ForegroundColor Gray
    }
}

# ---------------- 3) 提交并推送 ----------------
if ($Push) {
    Head "[3/3] Commit and push"
    Set-Location $RepoDir

    if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
        throw "git not found in PATH. Install Git for Windows first."
    }
    Say "git: $((git --version))"

    if (-not (Test-Path (Join-Path $RepoDir ".git"))) {
        & git init
        & git branch -M main
    } else {
        Say "existing .git found, reusing"
    }

    & git config user.name  $UserName
    & git config user.email $Email
    & git add -A

    Say "--- staged files ---"
    & git status --short

    $hasCommit = (& git rev-parse --verify HEAD 2>$null)
    if (-not $hasCommit) {
        & git commit -m "Qt sample 2.4: hybrid UI design with About tool button"
    } else {
        & git commit -m "Update Qt sample 2.4" 2>$null
        Say "(nothing to commit is fine)"
    }

    $remotes = & git remote
    if ($remotes -notcontains "origin") {
        & git remote add origin $Remote
    } else {
        & git remote set-url origin $Remote
    }
    Say "remote origin = $Remote"

    Say "checking remote reachability ..."
    & git ls-remote --heads origin 2>&1 | Select-Object -First 3

    Say "pushing ..."
    & git push -u origin main 2>&1

    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n  PUSH OK -> https://github.com/$UserName/Qtcourse" -ForegroundColor Green
    } else {
        Write-Host "`n  PUSH FAILED, see message above." -ForegroundColor Red
    }
}

if (-not $Deploy -and -not $Push -and -not $Test) {
    Write-Host "Nothing to do. Use -Deploy, -Test and/or -Push." -ForegroundColor Yellow
}

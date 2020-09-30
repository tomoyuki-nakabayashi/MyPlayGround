# Ubuntu環境構築

## 日本語入力

https://kinakoankon.net/ubuntu-20-04-japanese-input-ibus-fcitx-mozc/

- keyboard input method system -> IBus
- input method -> Japanese (Mozc)

## エディタ

vscode

## ワークスペース

https://sicklylife.jp/ubuntu/2004/settings.html#workspace

```
$ sudo apt install gnome-tweaks
```

- tweak -> workspace -> Static Workspaces
- tweak -> workspace -> Workspaces span displays

## ワークスペース切り替えショートカット

- Settings -> Keyboard Shortcuts -> Navigation -> Move window to workspace %

## 開発ツール

- git

## git設定

## GitHubへのSSH鍵登録

### SSH鍵の生成

https://docs.github.com/en/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent

SSH鍵の作成

```
$ ssh-keygen -t rsa -b 4096 -C "tomo.wait.for.it.yuki@gmail.com"
```

ssh-agentの起動

```
$ eval "$(ssh-agent -s)"
```

```
ssh-add ~/.ssh/id_rsa
```

### GitHubへの登録

https://docs.github.com/en/github/authenticating-to-github/adding-a-new-ssh-key-to-your-github-account

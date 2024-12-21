git clone https://github.com/kirin123kirin/.vscode
git clone https://github.com/kirin123kirin/.github
rmdir /s /q .vscode\.git
rmdir /s /q .github\.git

git commit -a -m "submodeles initial updates"
git push

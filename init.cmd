# remove all submodules (`git rm`) from .gitmodules
git submodule | cut -c43- | while read -r line; do (git rm "$line"); done

# delete all submodule sections from .git/config (`git config --local --remove-section`) by fetching those from .git/config
git config --local -l | grep submodule | sed -e 's/^\(submodule\.[^.]*\)\(.*\)/\1/g' | while read -r line; do (git config --local --remove-section "$line"); done

# delete metadata
rm -rf .git\modules\.github && rm .github\.git
rm -rf .git\modules\.vscode && rm .vscode\.git
git commit -a -m "submodeles initial updates"
git push

#!/bin/bash

# ==============================
#
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!!!!                                                        !!!!!
# !!!!!     THIS SHELL SCRIPT REQUIRES SSH ON YOUR ACCOUNT     !!!!!
# !!!!!                                                        !!!!!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
# # Generate a new key pair
# ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
#
# # Make sure to set name to 'id_rsa' !!!!
#
# # Add the SSH key to the SSH agent
# eval "$(ssh-agent -s)"
#
# # Add private SSH key to the SSH agent
# ssh-add ~/id_rsa
#
# # Copy the public key to clipboard
# cat ~/id_rsa.pub
#
# # Then go to GitHub:
# # - Go to settings
# # - Click "SSH and GPG keys"
# # - Click "New SSH key"
# # - Paste the public SSH key into the key field
# # - Give it a title
#
# # Configure Git to always use this key for GitHub:
# # Create or edit the file: ~/.ssh/config
#
# # Paste the following:
# Host github.com
#   HostName github.com
#   User git
#   IdentityFile ~/id_rsa
#   IdentitiesOnly yes
#
# # Test if SSH works in terminal, if it says something like
# # 'Hi username!....', then SSH is working successfully
# ssh -T git@github.com
#
# ==============================

# Current directory dir
ROOT_DIR=$(dirname "$(realpath "$0")")

# Change to the directory of the script
cd "$ROOT_DIR" ||
{
    echo "Failed to change directory to ROOT_DIR";
    read -p "Press enter to exit...";
    exit 1;
}

echo ""
echo "=============================="
echo ""

# Print the current directory
echo "Current directory: $(pwd)"

echo ""
echo "=============================="
echo ""

# Check if SSH agent is running and the SSH key works
eval "$(ssh-agent -s)"
ssh-add ~/id_rsa
if [ $? -eq 0 ]; then
    echo "SSH key added successfully!"
else
    echo "Failed to add SSH key!"
    read -p "Press enter to exit..."
    exit 1
fi

# Check if SSH authentication works
OUTPUT=$(ssh -T git@github.com 2>&1)
if echo "$OUTPUT" | grep -q "successfully authenticated"; then
    echo "SSH authentication works!"
else
    echo "SSH authentication failed!"
    read -p "Press enter to exit..."
    exit 1
fi

echo ""
echo "=============================="
echo ""

# Initialize Git at the current directory if it isnt already initialized
if [ ! -d ".git" ]; then
    echo "Git repository not found, initializing a new Git repository..."
    git init # Initializes git
    if [ $? -ne 0 ]; then
        echo "Failed to initialize Git repository."
        read -p "Press enter to exit..."
        exit 1
    fi
else
    echo "Git repository already initialized."
fi

echo ""
echo "=============================="
echo ""

# Pull latest changes before pushing
echo "Pulling latest changes from remote..."
previous_commit=$(git rev-parse HEAD)
pull_output=$(git pull origin "$branch_name" 2>&1)
new_commit=$(git rev-parse HEAD)
if [[ "$pull_output" == *"Already up to date."* ]]; then
    echo "Already up to date."
else
    echo "Pulled new changes from remote."

    echo ""
    echo "Recent commits pulled:"
    git log --oneline "$previous_commit..$new_commit"
fi

echo ""
echo "=============================="
echo ""

# Check for unstaged files
if ! git diff --quiet; then
    echo "Unstaged changes detected:"
    echo ""
    git diff --name-only
    echo ""
fi

# Check for staged files (to be commited)
if ! git diff --cached --quiet; then
    echo "Staged changes detected:"
    echo ""
    git diff --cached --name-only
    echo ""
fi

# Check for untracked files
untracked_files=$(git ls-files --others --exclude-standard)
if [ -n "$untracked_files" ]; then
    echo "Untracked files detected: "
    echo ""
    echo "$untracked_files"
fi

echo ""
echo "=============================="
echo ""

# Quit if no staged or unstaged or untracked files were detected
if git diff --quiet \
    && git diff --cached --quiet \
    && [ -z "$(git ls-files --others --exclude-standard)" ]; then
    echo "No changes to commit."
    read -p "Press enter to exit..."
    exit 0
fi

# Ask if the user wants to commit
read -p "Do you want to commit the changes? (y/n): " commit_choice
if [[ "$commit_choice" == "y" || "$commit_choice" == "Y" ]]; then
    # Stage the changes before committing
    echo "Staging changes..."
    git add .

    # Ask for commit message
    read -p "Enter commit message: " commit_message

    if [ -z "$commit_message" ]; then
        echo "No commit messages provided."
        read -p "Press enter to exit..."
        exit 1
    else
        git commit -m "$commit_message"
        if [ $? -ne 0 ]; then
            echo "Failed to commit changed."
            read -p "Press enter to exit..."
            exit 1
        fi
        echo "Changes commited successfully."
    fi
else
    echo "User chose not to commit changes."
    read -p "Press enter to exit..."
    exit 0
fi

echo ""
echo "=============================="
echo ""

# Detect current remote
current_remote=$(git remote get-url origin 2>/dev/null)
repo_name=$(basename "$ROOT_DIR")

if [ -n "$current_remote" ]; then
    # Extract the owner name from the SSH URL
    current_owner=$(echo "$current_remote" | cut -d':' -f2 | cut -d'/' -f1)
    read -p "Current owner: $current_owner. Stay on owner? (y/n): " owner_switch
    if [[ "$owner_switch" == "y" || "$owner_switch" == "Y" ]]; then
        echo "Stayed on owner '$current_owner'."
        new_url="git@github.com:$current_owner/$repo_name.git"
    else
        # Choose remote
        read -p "Enter owner name (username or organization): " github_owner
        new_url="git@github.com:$github_owner/$repo_name.git"

        echo "Setting remote 'origin' to: $new_url"
        git remote set-url origin "$new_url"
    fi
else
    echo "No remote 'origin' is set."

    # Choose remote
    read -p "Enter github owner (username or organization): " github_owner
    new_url="git@github.com:$github_owner/$repo_name.git"

    echo "Setting remote 'origin' to: $new_url"
    git remote add origin "$new_url"
fi

# Check if user has access
git ls-remote "$new_url" $>/dev/null
if [ $? -ne 0 ]; then
    echo "Remote repository '$new_url' does not exist or is inaccessible! Undoing commits."
    git reset --soft HEAD~1
    read -p "Press enter to exit..."
    exit 1
fi

echo ""
echo "=============================="
echo ""

# Choose branch
current_branch=$(git rev-parse --abbrev-ref HEAD)
read -p "Current branch: '$current_branch'. Stay on branch? (y/n): " branch_switch

# Stay on branch
if [[ "$branch_switch" == "y" || "$branch_switch" == "Y" ]]; then
    echo "Stayed on branch '$current_branch'."
    branch_name="$current_branch"
else
    # Change branch
    read -p "Enter branch name: " branch_name
    # If user already is on the same branch
    if [ "$current_branch" == "$branch_name" ]; then
        echo "User is already on branch '$branch_name'."
    else
        # Check if branch exists
        if git show-ref --quiet refs/heads/"$branch_name"; then
            echo "Using existing branch '$branch_name'"
            git checkout "$branch_name" &>/dev/null
        else
            read -p "Branch '$branch_name' does not exist. Create new branch? (y/n): " new_branch
            if [[ "$new_branch" == "y" || "$new_branch" == "Y" ]]; then
                echo "Creating new branch '$branch_name'"
                git checkout -b "$branch_name" &>/dev/null
            else
                echo "User chose not to create new branch. Undoing commits."
                git reset --soft HEAD~1
                read -p "Press enter to exit..."
                exit 1
            fi
        fi
    fi
fi

echo ""
echo "=============================="
echo ""

echo "Using remote '$new_url' and branch '$branch_name'."
echo ""

# Promt to push the changes
read -p "Do you want to push the changes to the remote repository? (y/n): " push_choice
if [[ "$push_choice" == "y" || "$push_choice" == "Y" ]]; then
    git push origin "$current_branch"
    if [ $? -ne 0 ]; then
        echo "Failed to push changes to remote repository. Undoing commits."
        git reset --soft HEAD~1
        read -p "Press enter to exit..."
        exit 1
    fi
    echo "Successfully pushed changes!"
fi

# Pause and wait for user input before closing
read -p "Press enter to exit..."

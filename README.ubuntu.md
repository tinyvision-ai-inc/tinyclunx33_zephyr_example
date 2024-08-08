# Complete Getting Started Guide    

This guide is adapted from the [Zephyr Getting Started](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) section and will cover:

- Set up a command-line Zephyr development environment on Ubuntu LTS: Either natively on a workstation, on Windows WSL, or in a vritual machine
- Get the source code.
- Build, flash, and run example applications.

  
## Update and install dependencies

Run the following commands in your terminal.

```
sudo apt update
sudo apt install --no-install-recommends git cmake ninja-build gperf \
    ccache dfu-util device-tree-compiler wget \
    python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
    make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1
```


## Install west and Python dependencies

West is the build tool used by Zephyr.

Install west globally on your machine:

```
pip3 install --user -U west pyelftools
echo 'export PATH=~/.local/bin:"$PATH"' >>~/.bashrc
source ~/.bashrc
west zephyr-export
```


## Install Zephyr SDK

```
# Download the Zephyr SDK in your home directory
cd ~/.local/
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_linux-x86_64.tar.xz
wget -O- https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/sha256.sum | shasum --check --ignore-missing

# Extract the Zephyr SDK bundle archive.
tar xvf zephyr-sdk-0.16.8_linux-x86_64.tar.xz

# Run the Zephyr SDK bundle setup script.
bash zephyr-sdk-0.16.8/setup.sh
```


## Request the access to `zephyr_private`

Now that we have successfully installed and verified all the dependencies and support packages needed to build our own zephyr project,
next we can move on to copy Zephyr and its modules to the project folder in our local machine.

Since we are currently at the development stage and our board support is not added to Zephyr mainstream repo, we will clone our private repo to local.
To do so first we need to make an ssh key pair in our local machine and upload the public key into github.

Follow the instructions and command given below to do the above mentioned,

```
# Generate an ssh key pair, press Enter when prompted for a passphrase
ssh-keygen -t ed25519 -C "you@example.com"

# Open the newly id_ed25519.pub file using the following command
cat ~/.ssh/id_ed25519.pub
```

Then copy the output and paste it into the "Key" field of this page:
<https://github.com/settings/ssh/new>

Then the access can be requested to tinyVision.ai Inc to be able to reach this page without 404 error:
<https://github.com/tinyvision-ai-inc/zephyr_private>


## Clone Zephyr and `tinyclunx33_zephyr_example`

```
# Download Zephyr into a directory and create ~/zephyrproject/.west/config
mkdir ~/zephyrproject
cd ~/zephyrproject
west init -l https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example.git
west update

# Install all the requirements to run Zephyr
west zephyr-export
pip3 install --user -r ~/zephyrproject/zephyr/scripts/requirements.txt
```


## Build `tinyclunx33_zephyr_example`

Now we can build our board specific example codes which already downloaded. 

```
# Build the example_usb_cdc_shell demo
cd ~/zephyrproject/tinyclunx33_zephyr_example/example_usb_cdc_shell 
source ~/zephyrproject/zephyr_private/zephyr-env.sh
west build -b tinyclunx33

# Load it onto the board
west flash
```

This will run the ecpprog command to load it onto your board.

You can now pursue with the `tinyclunx33_zephyr_example` instructions.

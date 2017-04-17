#!/bin/bash

# Tested on 64-bit Ubuntu 16.04/equivalent
# With credits to http://bit.ly/2oDU698 and http://bit.ly/2oUpD68 and http://bit.ly/2nziQhj

# Global/Configuration Vars:
GCC_TOOLCHAIN_LINK="https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2"
GCC_TOOLCHAIN_TEMP="/tmp/gcc-arm-embedded.tar.bz2"
TIVAWARE_LINK="https://www.dropbox.com/s/bf8fvmcehhs5ao7/SW-TM4C-2.1.4.178.exe?dl=1"
TIVAWARE_TEMP="/tmp/tivaware.zip"
GENERAL_TEMP="/tmp/"

INSTALL_LOC=~/TM4C-GNU

GCC_FOLDER_NAME="gcc-arm-none-eabi-5_4-2016q3" # Must match version in gcc link

username=""

output="true"

# Colours:
BOLD='\033[0;1m' #(OR USE 31)
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
GREEN='\033[0;32m'
BROWN='\033[0;33m'
RED='\033[1;31m'
NC='\033[0m' # No Color

# Helper Functions:
function print
{
    if [[ "$output" != "true" ]]; then return; fi

    N=0
    n="-e"

    if [[ "$*" == *" -n"* ]]; then
        N=1
        n="-ne"
    fi

    if [ "$#" -eq $((1 + $N)) ]; then
        echo $n $1
    elif [ "$#" -eq $((2 + $N)) ]; then
        printf ${2} && echo $n $1 && printf ${NC}
    else
        #printf ${RED} && echo "Error in print. Received: $*" && printf ${NC}
        printf ${RED} && echo "Received: $*" && printf ${NC}
    fi
}

function handleInputParams
{
    if [ -z "$1" ]; then        # Check if an install location was given
        print "No install location specified, defaulting to ${INSTALL_LOC}..." $BOLD
    else                    
        if [ -d "$1" ]; then    # If so, check that it actually exists
            INSTALL_LOC=$1
            print "Installing to ${INSTALL_LOC}" $PURPLE
        else
            print "Specified install location ($1) does not exist." $RED
            return 1
        fi
    fi

    return 0
}

function checkForSudo
{
    if [[ $UID != 0 ]]; then
        print "Please run this script with sudo:" $RED
        echo "sudo $0 $*"
        exit 1
    fi
}

function getRealUsername
{
    # Ask for username:
    print "Enter username: " $BOLD -n
    read username

    id -u "$username" > /dev/null 2>&1

    if [[ "$?" != 0 ]]; then
        print "Invalid username; please try again." $RED
        getRealUsername
        return $?
    fi

    print "Installing toolchain for ${username}..." $CYAN

    mkdir -p "${INSTALL_LOC}"

    return 0
}

function installDependencies
{
    print "Installing needed packages.." $PURPLE

    apt-get install -qq flex bison libgmp3-dev libmpfr-dev \
    libncurses5-dev libmpc-dev autoconf texinfo build-essential \
    libftdi-dev python-yaml zlib1g-dev libtool libc6:i386 \
    libncurses5:i386 libstdc++6:i386 libusb-1.0-0 libusb-1.0-0-dev \
    screen openocd

    return $?
}

function getGCCToolchain
{
    print "Downloading gcc-arm-embedded toolchain" $PURPLE && \
    wget -O "${GCC_TOOLCHAIN_TEMP}" "${GCC_TOOLCHAIN_LINK}" -q --show-progress && \

    print "Extracting..." $PURPLE && \
    tar -xjf "${GCC_TOOLCHAIN_TEMP}" -C "${INSTALL_LOC}/"

    return $?
}

function getTivaWare
{
    print "Downloading TivaWare..." $PURPLE && \
    wget -O "${TIVAWARE_TEMP}" "${TIVAWARE_LINK}" -q --show-progress && \

    print "Extracting TivaWare..." $PURPLE && \
    mkdir -p "${INSTALL_LOC}/TivaWare" && \
    unzip ${TIVAWARE_TEMP} -d ${INSTALL_LOC}"/TivaWare" && \

    print "Building TivaWare..." $PURPLE && \
    cd ${INSTALL_LOC}"/TivaWare" && \
    PATH=$PATH:"${INSTALL_LOC}/${GCC_FOLDER_NAME}/bin" && \
    make -j8

    return $?
}

function installLM4Flash
{
    print "Installing LM4Flash Tool" $PURPLE

    rm -r "${INSTALL_LOC}/lm4tools/"
    git clone https://github.com/utzig/lm4tools.git "${INSTALL_LOC}/lm4tools" && \
    cd "${INSTALL_LOC}/lm4tools/lm4flash" && \
    make -j8

    return $?
}

function installOpenOCD
{
    print "Installing OpenOCD" $PURPLE

    git clone git://git.code.sf.net/p/openocd/code openocd.git ${INSTALL_LOC}"/openocd" && \
    cd ${INSTALL_LOC}"/openocd" && \
    ./bootstrap && \
    ./configure --prefix=/usr --enable-maintainer-mode --enable-stlink --enable-ti-icdi && \
    make -j8 && \
    make install

    return $?
}

function addUdevRule
{
    print "Adding udev rule..." $PURPLE

    # Add udev rule:
    cat << EOF > /etc/udev/rules.d/99-tiva-launchpad.rules
ATTR{idVendor}=="15ba", ATTR{idProduct}=="0004", GROUP="plugdev", MODE="0660" # Olimex Ltd. OpenOCD JTAG TINY
ATTR{idVendor}=="067b", ATTR{idProduct}=="2303", GROUP="plugdev", MODE="0660" # Prolific Technology, Inc. PL2303 Serial Port
ATTR{idVendor}=="10c4", ATTR{idProduct}=="ea60", GROUP="plugdev", MODE="0660" # USB Serial
ATTR{idVendor}=="1cbe", ATTR{idProduct}=="00fd", GROUP="plugdev", MODE="0660" # TI Stellaris Launchpad
EOF
    
    # Reload udev rules:
    udevadm control --reload-rules

    # Try to create plugdev group in case it doesn't already exist
    # (we can safely ignore errors here)
    groupadd plugdev > /dev/null 1>&2

    # Add user to groups:
    usermod -aG plugdev "$username"
    usermod -aG dialout "$username"

    return 0
}

function addToPath
{
    print "Add gcc toolchain and lm4flash tool to PATH?"
    select yn in "Yes" "No"; do
        case $yn in
            Yes )  break;;
            No  )  return;;
        esac
    done

    # Add lm4flash to path via symlink (simpler):
    sudo ln -s ${INSTALL_LOC}"/lm4tools/lm4flash/lm4flash" /usr/local/bin/lm4flash

    # Add gcc toolchain to PATH via bashrc
    echo '# TM4C Toolchain:' >> ~/.bashrc
    echo "export PATH="'$PATH'":${INSTALL_LOC}/${GCC_FOLDER_NAME}/bin" >> ~/.bashrc

    return $?
}

function fin
{
    chown -R "${username}:${username}" "${INSTALL_LOC}"

    if [ "$1" != 0 ]; then
        print "Error occured; installation did not complete successfully." $RED
    else
        print "Installation complete. Enjoy!" $CYAN
        print "You may need to reboot for the udev rules to work properly." $CYAN
    fi
}

checkForSudo && \
getRealUsername && \
handleInputParams && \
installDependencies && \
getGCCToolchain && \
getTivaWare && \
installLM4Flash && \
# installOpenOCD && \ # OpenOCD w/tm4c supoprt should be available via apt now
addUdevRule && \
addToPath 

fin $?


##########################
# AUTHOR:  Rahul Butani  #
# DATE:    April 17, 2017 #
# VERSION: 0.0.1         #
##########################
#!/bin/bash

# Tested on 64-bit Ubuntu 16.04/equivilant
# With credits to http://bit.ly/2oDU698 and http://bit.ly/2oUpD68 and http://bit.ly/2nziQhj

# Global/Configuration Vars:
GCC_TOOLCHAIN_LINK="https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2"
GCC_TOOLCHAIN_TEMP="/tmp/gcc-arm-embedded.tar.bz2"
TIVAWARE_LINK="https://www.dropbox.com/s/bf8fvmcehhs5ao7/SW-TM4C-2.1.4.178.exe?dl=1"
TIVAWARE_TEMP="/tmp/tivaware.zip"
GENERAL_TEMP="/tmp/"

INSTALL_LOC="~/TM4C-GNU"

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

function installDependencies
{
    print "Installing" $PURPLE

    apt-get install -qq flex bison libgmp3-dev libmpfr-dev \
    libncurses5-dev libmpc-dev autoconf texinfo build-essential \
    libftdi-dev python-yaml zlib1g-dev libtool libc6:i386 \
    libncurses5:i386 libstdc++6:i386 libusb-1.0-0 libusb-1.0-0-dev \
    screen openocd

    return $?
}

function getGCCToolchain
{
    print "Downloading gcc-arm-embedded toolchain" $PURPLE

    wget ${GCC_TOOLCHAIN_LINK} -o ${GCC_TOOLCHAIN_TEMP} && \
    tar -xvf ${GCC_TOOLCHAIN_TEMP} -C ${INSTALL_LOC}

    return $?
}

function getTivaWare
{
    print "Downloading TivaWare" $PURPLE

    wget ${TIVAWARE_LINK} -o ${TIVAWARE_TEMP} && \
    mkdir ${INSTALL_LOC}"/TivaWare" && \
    unzip ${TIVAWARE_TEMP} -d ${INSTALL_LOC}"/TivaWare" && \
    cd ${INSTALL_LOC}"/TivaWare" && \
    make

    return $?
}

function installLM4Flash
{
    print "Installing LM4Flash Tool" $PURPLE

    git clone https://github.com/utzig/lm4tools.git ${INSTALL_LOC}"/lm4tools" && \
    cd ${INSTALL_LOC}"/lm4tools/lm4flash" && \
    make

    return $?
}

function installOpenOCD
{
    print "Installing OpenOCD" $PURPLE

    git clone git://git.code.sf.net/p/openocd/code openocd.git ${INSTALL_LOC}"/openocd" && \
    cd ${INSTALL_LOC}"/openocd" && \
    ./bootstrap && \
    ./configure --prefix=/usr --enable-maintainer-mode --enable-stlink --enable-ti-icdi && \
    make && \
    make install

    return $?
}

function addUdevRule
{
    
}

function addToPath
{
    print "Add gcc toolchain, "
    select yn in "Yes" "No"; do
        case $yn in
            Yes )  break;;
            No  )  return;;
        esac
    done
}

function fin
{
    if [ "$1" == 0 ]; then
        print "Error occured; installation did not complete successfully." $RED
    else
        print "installation complete. Enjoy!" $CYAN
    fi
}

checkForSudo && \
handleInputParams && \
installDependencies && \
getGCCToolchain && \
getTivaWare && \
installLM4Flash && \
# installOpenOCD && \
addToPath 

fin $?


##########################
# AUTHOR:  Rahul Butani  #
# DATE:    April 4, 2017 #
# VERSION: 0.0.0         #
##########################
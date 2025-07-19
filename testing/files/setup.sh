sudo apt update
sudo apt install build-essential valgrind -y

wget https://ftp.gnu.org/gnu/inetutils/inetutils-2.0.tar.xz
tar -xf inetutils-2.0.tar.xz
cd inetutils-2.0
./configure --prefix=/usr/local
sudo make install


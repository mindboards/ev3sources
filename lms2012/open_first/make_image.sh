#/bin/sh

echo "Creating Filesystem..."
./scripts/make_ev3_os.sh

echo "Creating Image..."
./scripts/make_image.pl

gcc ex21.c

echo identical:

./a.out files/textComparison/identical/chandler/chandler1.txt files/textComparison/identical/chandler/chandler2.txt
echo $?
./a.out files/textComparison/identical/ross/ross1.txt files/textComparison/identical/ross/ross2.txt
echo $? 

echo different:

./a.out files/textComparison/different/break/rachel.txt files/textComparison/different/break/ross.txt
echo $?
./a.out files/textComparison/different/french/joey.txt files/textComparison/different/french/phoebe.txt
echo $?
./a.out files/textComparison/different/names/name1.txt files/textComparison/different/names/name2.txt
echo $?

echo similar:

./a.out files/textComparison/similar/moo/joey1.txt files/textComparison/similar/moo/joey2.txt
echo $?
./a.out files/textComparison/similar/share/joey1.txt files/textComparison/similar/share/joey2.txt
echo $?

echo errors:

echo args:
./a.out
echo  

echo open:
./a.out sgsrgsfews wefgwegtwsf
echo  

/*
This program outputs the '99 bottles of beer' song.

It is taken from http://www.99-bottles-of-beer.net/language-c-844.html
*/
int main(void)
{
		// the counter indicates how many bottles are left.
        int b;

        // loop to destroy them one after another
        for (b = 99; b >= 0; b--) {
        		// print the respective stanza.
                switch (b) {
                case 0:
                        printf("No more bottles of beer on the wall, no more bottles of beer.\n");
                        printf("Go to the store and buy some more, 99 bottles of beer on the wall.\n");
                        break;
                case 1:
                        printf("1 bottle of beer on the wall, 1 bottle of beer.\n");
                        printf("Take one down and pass it around, no more bottles of beer on the wall\n");
                        break;
                default:
                        printf("%d bottles of beer on the wall, %d bottles of beer.\n", b, b);
                        printf("Take one down and pass it around, %d %s of beer on the wall.\n"
                                ,b - 1
                                ,((b - 1) > 1)? "bottles" : "bottle");
                        break;
                }
        }

        return 0;
}





union Commands
{
	struct
	{
		/* data */
		byte mode;
		byte type;

	}__attribute__((packed));
	
};
union Config
{
	struct
	{
		/* data */
	}__attribute__((packed));
	
};



/*
	command layout
	
	LED
	|---------------|---------------|---------------|---------------|---------------|---------------|
	|X X X X X X X X|X X X X X X X X|X X X X X X X X|R R R R R R R R|G G G G G G G G|B B B B B B B B|
	|---------------|---------------|---------------|---------------|---------------|---------------|
		  Read/Write        Type             ID           Red            Green           Blue

*/


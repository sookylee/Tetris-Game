#include "tetris.h"

static struct sigaction act, oact;

treePointer make_node(){
	treePointer node=(treePointer)malloc(sizeof(Node));
	if(node==NULL) {
		printw("malloc Error!\n");
		exit(100);
	}
	else {
		node->left=NULL;
		node->right=NULL;
	}
	return node;
}


int main(){
	int exit=0;

	initscr();
	start_color();	//using color
	
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_HARD: hardplay(); break;
		default: break;
		}
	}
	
	if(writeFlag){
		writeRankFile();
		writeFlag=0;
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;	//두번째 다음 블록을 임의로 생성
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);	//두번째 next block을 보여주는 테두리

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(17,WIDTH+10);
	printw("SCORE");
	DrawBox(18,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		command=SPACE;
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	case SPACE:
		while(CheckToMove(field,nextBlock[0],blockRotate,++blockY,blockX));
		blockY--;
		
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(19,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;

	//color set
	for(i=1;i<8;i++){
		if(i==7){
			init_color(COLOR_CYAN,500,0,500);
			init_pair(7,6,6);
		}
		else
			init_pair(i,i,i);
	}

	for( i = 0; i < 4; i++ ){		
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(COLOR_PAIR(nextBlock[1]+1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[1]+1));
			}
			else printw(" ");
		}
	}

	for( i = 0; i < 4; i++ ){
                move(10+i,WIDTH+13);
                for( j = 0; j < 4; j++ ){
                        if( block[nextBlock[2]][0][i][j] == 1 ){
                                attron(COLOR_PAIR(nextBlock[2]+1));
                                printw(" ");
                                attroff(COLOR_PAIR(nextBlock[2]+1));
                        }
                        else printw(" ");
                }
        }

}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;

	//color set
	init_color(COLOR_RED,700,150,150);	

	for(i=1;i<8;i++){
		if(i==7){
			init_color(COLOR_CYAN,500,0,500);
			init_pair(7,7,6);
		}
		else
			init_pair(i,7,i);
	}
		

	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				
				attron(COLOR_PAIR(blockID+1));
				printw("%c",tile);
				attroff(COLOR_PAIR(blockID+1));
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch('o');
	//addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
	//	addch(ACS_HLINE);
	//addch(ACS_URCORNER);
	addch('-');
	addch('o');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		//addch(ACS_VLINE);
		addch('|');
		move(y+j+1,x+width+1);
		//addch(ACS_VLINE);
		addch('|');
	}
	move(y+j+1,x);
	//addch(ACS_LLCORNER);
	addch('o');
	for(i=0;i<width;i++)
		addch('-');
	//	addch(ACS_HLINE);
	//addch(ACS_LRCORNER);
	addch('o');
}

void play(){
	int command;
	score=0;
	timed_out=0;

	clear();

	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

void hardplay(){
	int command;
	score=0;
	timed_out=0;

	clear();

	act.sa_handler=BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();


        move(0,WIDTH+7);
        attron(A_REVERSE);
        printw("Welcome to HARD mode!");
        attroff(A_REVERSE);

	do{
		if(timed_out==0){
			ualarm(300000,0);
			timed_out=1;
	}

	command=GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
	
			return;
		}
	}while(!gameOver);
	
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n\n");
	printw("5. play HARD mode\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	for(i=0;i<BLOCK_HEIGHT;i++)
		for(j=0;j<BLOCK_WIDTH;j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(f[blockY+i][blockX+j]) return 0;
				if(blockY+i > HEIGHT-1) return 0;
				if(blockX+j < 0) return 0;
				if(blockX+j > WIDTH-1) return 0; 	
			}
		}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	/*remove the block*/
	int ty=blockY,tx=blockX,tr=blockRotate;
	int shadowY=ty;
	switch(command){
		case KEY_UP: tr=(tr+3)%4; break;
		case KEY_DOWN: ty--;break;
		case KEY_RIGHT: tx--;break;
		case KEY_LEFT: tx++;break;
		default: break;
	}
	while(CheckToMove(field,currentBlock,tr,++shadowY,tx));
	for(int i=0;i<BLOCK_HEIGHT;i++)
		for(int j=0;j<BLOCK_WIDTH;j++)
			if(block[currentBlock][tr][i][j]){
				//remove the previous block
				move(i+ty+1,j+tx+1);
				printw("%c",'.');
				//remove the previous shadow
				move(i+shadowY,j+tx+1);
				printw("%c",'.');
			}
	/*draw block after command*/
	DrawBlock(blockY,blockX,currentBlock,blockRotate,' ');
	DrawShadow(blockY,blockX,currentBlock,blockRotate);
}

void BlockDown(int sig){
	// user code
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)){
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,++blockY,blockX);
	}
	else{
		if(blockY==-1) gameOver=1;

		AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score+=DeleteLine(field);
		PrintScore(score);
	
		nextBlock[0]=nextBlock[1];
		nextBlock[1]=nextBlock[2];
		nextBlock[2]=rand()%7;
		DrawNextBlock(nextBlock);

		blockRotate=0;
		blockY=-1; blockX=WIDTH/2-2;

		DrawField();
	}
	timed_out=0;
}

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int touched=0;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(block[currentBlock][blockRotate][i][j]){
				f[blockY+i][blockX+j]=1;
				//바닥이거나 바로 아래에 블록이 있을 때 touched 증가
				if((blockY+i==HEIGHT-1) || (f[blockY+i+1][blockX+j]))
					touched++;
			}
	score+=touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int count=0;
	int line[HEIGHT];
	int i,j;

	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++){
			if(!(f[i][j])) break;
			if(j==WIDTH-1) {
				count++;
				line[count-1]=i;
			}
		}
	for(int k=0;k<count;k++){
		for(i=line[k];i>0;i--)
			for(j=0;j<WIDTH;j++)
				f[i][j]=f[i-1][j];
	}
	return count*count*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int ty=y;
	while(CheckToMove(field,blockID,blockRotate,++ty,x));	//더 이상 내려갈 수 없는 위치 찾기
	
	DrawBlock(ty-1,x,blockID,blockRotate,'/'); 
}

void createRankList(){
	// user code
	FILE *fp;
	int i,score;
	
	ROOT=NULL;	node_num=0;
	
	fp=fopen("rank.txt","r");
	if(!feof(fp))
		fscanf(fp,"%d",&node_num);

	for(i=0;i<node_num;i++){
		char name[NAMELEN];
		fscanf(fp,"%s %d",name,&score);
		ROOT=insert(&ROOT,name,score);
	}
	fclose(fp);
	return;
}

/*making AVL TREE*/
int get_height(treePointer node){
	int height=0;
	if(node != NULL)
		height=1+MAX(get_height(node->left),get_height(node->right));
	return height;
}

int balanced_num(treePointer node){
	if(node==NULL) return 0;
	else return get_height(node->left)-get_height(node->right);
}
// rotate functions
treePointer LL(treePointer parent){
	treePointer child=parent->left;

	parent->left=child->right;
	child->right=parent;

	return child;
}

treePointer RR(treePointer parent){
	treePointer child=parent->right;
	
	parent->right=child->left;
	child->left=parent;

	return child;	
}

treePointer LR(treePointer parent){
	treePointer child=parent->left;
	
	parent->left=RR(child);
	return LL(parent);
}

treePointer RL(treePointer parent){
	treePointer child=parent->right;

	parent->left=LL(child);
	return RR(parent);
}

treePointer rotate(treePointer *node){
	int num=balanced_num(*node);

	if(num>1){
		if(balanced_num((*node)->left) > 0)
			*node=LL(*node);
		else	
			*node=LR(*node);
	}
	else if(num<-1) {
		if(balanced_num((*node)->right) > 0)
			*node=RL(*node);
		else
			*node=RR(*node);
	}
	return (*node);
}

treePointer insert(treePointer *root,char *name, int score){
	treePointer node=make_node(); 
	strcpy(node->name, name);
	node->score=score;

	if(*root==NULL)
		*root=node;
	
	else	{ //큰 순서대로 저장하기. BST와 반대로 크면 left에 저장
		if((*root)->score >= score) 
			(*root)->right = insert(&((*root)->right), name,score);
		else
			(*root)->left = insert(&((*root)->left),name,score);
		*root=rotate(root);
	}
	return (*root);	
}

void inorderTraversal(FILE *fp, int *count, treePointer node, int x, int y,int deleteFlag){
	if(node==NULL) return;
	else{	
		inorderTraversal(fp, count, node->left,x,y,deleteFlag);
		
		if(deleteFlag){
			if(find_num){
				PAR=node;
				find_num=0;
				return;
			}
		}
		
		(*count)++;
		if(fp!=NULL){
		fprintf(fp,"%s \t%d\n",node->name,node->score);
		}
		else if(((*count) >= x) && ((*count) <= y)){
			if(deleteFlag){
				CURR=node;
				find_num++;
				return;
			}
			else printw("  %-17s\t  |  %d\n",node->name,node->score);
		}
		else if(!deleteFlag) return;
		inorderTraversal(fp, count, node->right,x,y,deleteFlag);
		
		if(deleteFlag){
			if(find_num){
				PAR=node;
				find_num=0;
				return;
			}
		}
	}
	return;	
}

void inorderTraversal_name(char *name, treePointer node){
	if(node==NULL) return;
	else{
		inorderTraversal_name(name,node->left);
		if(!strcmp(node->name,name)){	//찾는 이름과 node의 이름이 일치하면
			find_num++;
			printw("  %-17s\t  |  %d\n",name,node->score);
		}
		inorderTraversal_name(name,node->right);
	}
	return;
}

void rank(){
	// user code
	char command;
	int exitFlag=0;
	int clearFlag=0;	
		
	do{
		clearFlag=0;
		clear();
		printw("1. list ranks from X to Y\n");
		printw("2. list ranks by a specific name\n");
		printw("3. delete a specific rank\n\n");
		/* 추가 기능. 레포트에 기술할 것*/
		printw("4. find the name who got the score\n");
		printw("5. clear\n");
		printw("          PRESS 'q' TO EXIT   \n");
		printw("=========================================\n");

		while(!exitFlag){
			command=wgetch(stdscr);
			switch(command){
			case '1': rank_1(); break;
			case '2': rank_2(); break;
			case '3': rank_3(); break;
			
			case '4': rank_4(); break;
			case '5': clearFlag=1; break;
			case 'Q':
			case 'q': exitFlag=1;
				break;
			default: break;
			}
			if(clearFlag) break;
		}
	}while(!exitFlag);

	return;
}

void rank_1(){
	
	int x=-2,y=-2;
	int count=0;
	
	echo();
	printw("X: ");	scanw("%d",&x); 
	printw("Y: ");	scanw("%d",&y);

	noecho();
	printw("\n          name             |    score   \n");
	printw("--------------------------------------------\n");
	
	if(x==-2)	x=1;
	if(y==-2 || y > node_num)	y=node_num;

	if(x > y){
	 printw("search failure: no rank in the list\n");	return;
	}
	if(x<0) {
		printw("search failure: boundary error\n");	return;
	}
	inorderTraversal(NULL,&count,ROOT,x,y,0);
	
	return;
}

void rank_2(){
	char name[NAMELEN];
	find_num=0;	//initialize
	
	printw("input the name: ");
	echo();
	scanw("%s",&name);
	noecho();

        printw("\n          name             |    score   \n");
        printw("--------------------------------------------\n");
	
	inorderTraversal_name(name,ROOT);
	if(!find_num)
		printw("\nsearch failure: no name in the list\n");
	
	return;	
}

void rank_3(){
	int rank=0;
	int count=0;
	//initialize
	find_num=0;
	CURR=NULL; PAR=NULL;
	writeFlag=0;

	printw("input the rank: ");
	echo();
	scanw("%d",&rank);
	noecho();
	
	if(!node_num)
		printw("search failure: the list of rank is EMPTY\n");
	else {
		if(rank<=0 || rank > node_num){
			printw("search failure: the rank not in the list\n");
		}
		else {
			inorderTraversal(NULL,&count,ROOT,rank,rank,1);
			delete_node();
			printw("\nresult: the rank deleted\n");
			writeFlag=1;
		}
	}
	return;
}

void rank_4(){
	int find_score=-1;
	treePointer now=ROOT;
	find_num=0;

	printw("input the SCORE to find: ");
	echo();
	scanw("%d",&find_score);
	noecho();
	
	if(!node_num || find_score<0)
		printw("\nfailure: no available value\n");
	else{
		printw("\n          name             |    score   \n");
		printw("--------------------------------------------\n");
		
		for(;now;){
			if(now->score > find_score)
				now=now->right;
			else if(now->score < find_score)
				now=now->left;
			else {
				printw("  %-17s\t  |  %d\n",now->name,now->score);
				find_num++;
				now=now->right;
			}
		}
		if(!find_num)
			printw("\nsearch failure: the score is not on the rank\n");		
	}
	return;
}

void delete_node(){
	treePointer temp,pre;

	if(CURR==NULL) ;
	else {
		if(!((CURR->left) && (CURR->right))){	//curr의 자식 노드가 2개가 아닐때
			if(CURR->left) temp=CURR->left;
			else if(CURR->right) temp=CURR->right;
			else temp=NULL;
			
			if(PAR==NULL) ROOT=temp;
			else if(PAR->left==CURR) PAR->left=temp;
			else PAR->right=temp;
			
			free(CURR);
		}
		else {	// 자식 노드가 2개일 때
			pre=CURR;	temp=CURR->left;
			if(temp->right!=NULL){
				pre=pre->left;
				for(temp=temp->right;temp->right!=NULL;temp=temp->right)
					pre=pre->right;
				pre->right=temp->left;
			}
			else pre->left=temp->left;

			strcpy(CURR->name,temp->name);
			CURR->score=temp->score;

			free(temp);
		}
		ROOT=rotate(&ROOT);
		node_num--;
	}
	return;
}

void writeRankFile(){
	// user code
	FILE *fp;
	int count=0;
	
	fp=fopen("rank.txt","w");
	
	if(node_num){
		fprintf(fp,"%d\n",node_num);
		inorderTraversal(fp,&count,ROOT,1,node_num,0);
	}
	fclose(fp);
	
	return;
}

void newRank(int score){
	// user code
	char name[NAMELEN];
	if(gameOver){
		clear();
		printw("Your name: ");
		echo();
		scanw("%s",name);
		noecho();

		node_num++;

		ROOT=insert(&ROOT,name,score);
		writeRankFile();
		refresh();
	}
	return;
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}

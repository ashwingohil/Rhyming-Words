#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <new>
#include <limits>
#include "Arrays.h"
#include "Dynarr.h"

using namespace std;

int users=0;
char defch='\0';
short int defnum=-1;


class Words{

	#define HMAX 40
	#define DEPTH 100
	#define AZ 26
	#define SEPERATOR '|'
	#define FILLER '#'
	#define REXT "_root.txt"
	#define IEXT "_inflected.txt"
	#define FILL "########################################" //len of HMAX
	
	protected:

	string username;
	bool updated_root[AZ];
	bool updated_inflexion[AZ];
	bool master_updated;
	bool existent_roots[AZ];
	bool existent_inflexions[AZ];
	int words_each_root[AZ][2];
	int wordsinflected_each_root[AZ][2];

	protected:
	short int LETTER(char ch);
	short int permissible_set(char ch);
	short int fileoperations(char root, bool inflexion, short int *filestatus, string mode="scan", char *storage=&::defch, short int *each_len=&::defnum);


	private:

	static short int totalobjects;
	char vocab_word[DEPTH][HMAX];
	char init_word[DEPTH][HMAX];
	char term_word[DEPTH][HMAX];
	
	short int vocab_each_len[DEPTH];
	short int init_each_len[DEPTH];	
	short int term_each_len[DEPTH];
	
	short int vocab_word_counts;
	short int init_word_counts;
	short int term_word_counts;
	
	short int vocab_lower_indices[AZ][2];
	short int term_lower_indices[AZ][2];
	short int init_lower_indices[AZ][2];

	private:

	void array_indexer(char *storage, signed short int *indexer_ptr, short int counts, short int hlen);
	int get_index_splitpoint(char ch, short int *indexer_ptr);
	int scanforunique(char *storage, short int *each_len, int counts); //coupled for push_user
	int lexicompare(int i, int j, char *storage, short int *each_len);
	int sort(char *storage, short int *each_len, int counts, bool bounce=1);
	short int stream_permissible_set(char ch);
	int updatedata_words(string &recv, bool inflected=0);
	int create_stringsbyroot(string s1, string s2);
	void initiater_wordcounts_onfile();


	public:

	Words();
	~Words();
	int passdata(istream &input,ostream &output);
	void push_user_datums(string word);
	void AssignUser_to_object(string name);
	void show_usernodeinfo();
	void create_object(const Words &object);


	friend void ADD_USER();
	friend void FREE_OBJECTS();
};

short int Words::totalobjects;

short int Words::LETTER(char ch){
	short int x;
	if('a'<= ch && ch <='z'){
		if((int)'a'== 97){
			x = (short int)(ch*1-97+1);
			return x;
		}
	}
	else return 0;
	//returns numberofthe letter
	//return ch*(short int)1-(short int)97+(short int)1;
}


short int Words::permissible_set(char ch){
	if('a' <= ch && ch <= 'z') return 1;
	else if(ch==SEPERATOR) return 1;
	else return 0;
}


short int Words::fileoperations(char root, bool inflexion, short int *filestatus, string mode, char *storage, short int *each_len){

	char operation, ch;
	ios::iostate i;
	short int root_numeric;
	int *type;
	int hlen,row,singleton,eachcounts;
	ifstream readfile;
	string fname;
	
	root_numeric = (short int)LETTER(root);
	fname=root;
	if(inflexion){
		fname = fname + IEXT;
		type=&wordsinflected_each_root[0][0];
	}
	else if(!inflexion){
		fname = fname + REXT;
		type=&words_each_root[0][0];
	}
	readfile.open(fname.c_str(),ios::in);
	if(mode=="load" && &*storage!=&defch && &*each_len!=&defnum) operation='l'; //used & for address comparision
	if(mode=="scan" && &*storage==&defch && &*each_len==&defnum) operation='c';
	
	if(readfile.is_open()){
		switch(operation){
			case 'c':
				*(type+2*root_numeric-2)=root_numeric;
				while(readfile.get(ch)){
					if(permissible_set(ch) && ch!=SEPERATOR){
						if(readfile.peek()==(int)SEPERATOR) *(type+2*root_numeric-2+1)=*(type+2*root_numeric-2+1)+1;
					}
				}
			break;
			
			case 'l':
			eachcounts=0;
			row=0;
			hlen=HMAX;
			singleton=0;
			while(readfile.get(ch)){
						if(permissible_set(ch) && ch!=SEPERATOR){
							if(permissible_set(readfile.peek())){
								*(storage+row*hlen+singleton)=ch;
								singleton=singleton+1;
								eachcounts=eachcounts+1;
							}
							if(readfile.peek()==(int)SEPERATOR){
									*(each_len+row)=eachcounts;
									row=row+1;
									singleton=0;
									eachcounts=0;
							}
						}
			}
			break;
		}

	}
	else{
		
		i=readfile.rdstate();
		if(i & ios::eofbit) *filestatus=1;
		else if(i & ios::failbit) *filestatus=2; //when file cannot be opened, file does not exist
		else if(i & ios::badbit) *filestatus=3;
		readfile.clear();
		return 0; //this is my final return if filestatus is not alright. error bit occurred.
	}
	
	*filestatus=0;
	readfile.close();
	return 1; //this is my final return with filestatus as 0 being allright. no error bit
}


void Words::array_indexer(char *storage, signed short int *indexer_ptr, short int counts, short int hlen){
	short int numch;
	for(short int row=0;row<counts;row++){
		numch=LETTER(*(storage+row*hlen+0));
		*(indexer_ptr+(numch-1)*2+0)=numch;
		*(indexer_ptr+(numch-1)*2+1)=row;
	}
}


int Words::get_index_splitpoint(char ch, short int *indexer_ptr){
	
	short int rootnumeric,zeroth;
	zeroth=-1;
	int index=-1;
	rootnumeric=LETTER(ch);
	cout<<"MY ROOT NUMERIC : "<<rootnumeric<<"\n";
	if(*(indexer_ptr+2*(rootnumeric-1)+0)==-1){
		for(short int i=0;i<26;i++){
			if(*(indexer_ptr+2*i+1)!=-1){
				zeroth=i;
				break;
			}
		}
		if(zeroth==-1) return 0; //none values having index in array, it is the very first value
		else if(zeroth!=-1){
				if(rootnumeric != 1){
					for(short int i=rootnumeric-1;i>=0;i--){
						if(*(indexer_ptr+2*i+1)!=-1){
							return *(indexer_ptr+2*i+1)+1;
						}
					}
					for(short int i=rootnumeric-1;i<26;i++){
						if(*(indexer_ptr+2*i+1)!=-1){
							return 0;
						}
					}
				}
				else if(rootnumeric == 1){
					if(*(indexer_ptr+2*(rootnumeric-1)+1)!=-1) return *(indexer_ptr+2*(rootnumeric-1)+1)+1;
					else return 0;
				}
		}
	}
	else if(*(indexer_ptr+2*(rootnumeric-1)+0)!=-1) return *(indexer_ptr+2*(rootnumeric-1)+1)+1; //than least will be 0  means from 1 is the answer
}


/*
void Words::shiftregion(char *storage, short int by, short int from){
	//this need to be made, create space by index split point
}
*/


int Words::scanforunique(char *storage, short int *each_len, int counts){
	//exclusive to push_user . can be rewritten
	int last,j;
	last=counts-1;
	j=0;
	while(j<counts-1){
		if(lexicompare(last,j,storage,each_len)==-1) return 0;
		j++;
	}
	return 1; //last unique
}


int Words::lexicompare(int i, int j, char *storage, short int *each_len){
	
	short int same=0;
	short int comparatorlen=0;
	short int k=1;
	int result=0;
			
		if(*(storage+i*HMAX+0)==*(storage+j*HMAX+0)){
			same=same+1;
			if(*(each_len+i)==1 && *(each_len+i)<*(each_len+j)) return result=0;
			else if(*(each_len+i)==1 && *(each_len+i)==*(each_len+j)) return result=-1;
			if(*(each_len+i)>=*(each_len+j)) comparatorlen=*(each_len+j);
			else comparatorlen=*(each_len+i);
				while(k<comparatorlen){
					if(*(storage+i*HMAX+k)>*(storage+j*HMAX+k)) return result=1;
					else if(*(storage+i*HMAX+k)<*(storage+j*HMAX+k)) return result=0;
					else if(*(storage+i*HMAX+k)==*(storage+j*HMAX+k)) same=same+1;
					k=k+1;
				}
				if(same==*(each_len+i) && *(each_len+i)<*(each_len+j)) return result=0;
				else if(same==*(each_len+j) && *(each_len+i)>*(each_len+j)) return result=1;
				else if(same==*(each_len+i) && same==*(each_len+j)) return result=-1;
		}		
		else if(*(storage+i*HMAX+0)>*(storage+j*HMAX+0)) return result=1;
		else if(*(storage+i*HMAX+0)<*(storage+j*HMAX+0)) return result=0;
	
	//1 is greater,0 is lesser and -1 is same 
}


int Words::sort(char *storage, short int *each_len, int counts, bool bounce){
	
	int unique=1;
	int lexiresult;
	int temparraylen=0;
	char temparray[1][HMAX];
	const char *fill=FILL;
	memmove(&temparray[0][0],(const char *)fill,HMAX*sizeof(char));
	
	for(int i=0;i<counts;i++){
		if(permissible_set(*(storage+i*HMAX+0)) && *(storage+i*HMAX+0)!=SEPERATOR){
		for(int j=i+1;j<counts;j++){
			if(permissible_set(*(storage+j*HMAX+0)) && *(storage+j*HMAX+0)!=SEPERATOR){
				lexiresult=lexicompare(i,j,storage,each_len);
			if(lexiresult==1){
				memmove(&temparray[0][0],(char *)storage+i*HMAX+0,*(each_len+i)*sizeof(char));
				temparraylen=*(each_len+i);
				memmove((char *)storage+i*HMAX+0,(const char*)fill,HMAX*sizeof(char));
				memmove((char *)storage+i*HMAX+0,(char *)storage+j*HMAX+0,*(each_len+j)*sizeof(char));
				*(each_len+i)=*(each_len+j);
				memmove((char *)storage+j*HMAX+0,(const char*)fill,HMAX*sizeof(char));
				memmove((char *)storage+j*HMAX+0,&temparray[0][0],temparraylen*sizeof(char));
				*(each_len+j)=temparraylen;
				memmove(&temparray[0][0],(const char*)fill,HMAX*sizeof(char));
				temparraylen=*(each_len+j);
			}
			else if(lexiresult==-1){
				if(bounce==0){
					memmove((char *)storage+j*HMAX+0,(const char*)fill,HMAX*sizeof(char));
					*(each_len+j)=0;
					unique=0;
					}
				else return 0; //was not unique
			}
		  }
		}
	 }
  }	

	if(unique) return 1; //means there was uniqueness
	else return 0; //was not unique
}


void Words::push_user_datums(string word){
	
	short int rows;
	char seperator;
	char *storage;
	short int *each_len,*counts;
	signed short int *indexer_ptr;
	seperator=*(word.data());

	switch(seperator){
		case '.':
			//vocab
			storage=&vocab_word[0][0];
			each_len=vocab_each_len;
			counts=&vocab_word_counts;
			indexer_ptr=&vocab_lower_indices[0][0];
			break;
		case '_':
			//root
			storage=&init_word[0][0];
			each_len=init_each_len;
			counts=&init_word_counts;
			indexer_ptr=&init_lower_indices[0][0];
			break;
		case '+':
			//transient
			break;
		case '~':
			//terminating
			storage=&term_word[0][0];
			each_len=term_each_len;
			counts=&term_word_counts;
			indexer_ptr=&term_lower_indices[0][0];
			break;
	}
			
	word.erase(0,1);
	const char *cur_ptr=word.c_str();
	//cout<<"READING CONST "<<*p;
	switch(*counts){
		case 0:
			rows = *counts+1;
			memmove((char *)storage,(const char *)cur_ptr,sizeof(char)*word.length());
			*(each_len+rows-1)=word.length();
			*counts=*counts+1;
			array_indexer(storage,indexer_ptr,*counts,HMAX);
		break;
				
		default:
			rows=*counts+1;
			memmove((char *)storage+(rows-1)*HMAX+0,(const char *)cur_ptr,sizeof(char)*word.length());
			*(each_len+rows-1)=word.length();
			*counts=*counts+1;
			cout<<"SCAN : "<<scanforunique(storage,each_len,*counts)<<"first char "<<*cur_ptr;
			cout<<"\n";
			cout<<"INDEX : "<<get_index_splitpoint(*(cur_ptr),indexer_ptr)<<"\n";
			if(!sort(storage,each_len,*counts)){
					const char *fill=FILL;
					memmove((char *)storage+(rows-1)*HMAX+0,(const char *)fill,HMAX*sizeof(char));
					*(each_len+(rows-1))=0;
					*counts=*counts-1;
					
			}
			array_indexer(storage,indexer_ptr,*counts,HMAX);
	}
		
}


short int Words::stream_permissible_set(char ch){
	if('a' <= ch && ch <= 'z') return 1;
	else if(ch=='\t' || ch=='\n' || ch==' ') return -1;
	else return 0;
}


int Words::updatedata_words(string &recv, bool inflected){
	if(recv.empty()) return 0;
	if(inflected!=0 && inflected!=1) return 0;
	
	const char *p=recv.data();
	char *storage, root;
	short int filestatus;
	int row, k=0, depth=0, pushsize=0;
	string fname;
	root=*(p+1);
	fname=root;
	if(inflected==0) fname=fname+REXT;
	else fname=fname+IEXT;
	while(k<recv.length()){
		if(*(p+k)=='|' && k!=recv.length()-1) depth=depth+1;
		k++;
	}
	switch((int)inflected){
		case 0:
			if(existent_roots[LETTER(*(p+1))-1]){
				depth = depth + words_each_root[LETTER(*(p+1))-1][1];
				row=words_each_root[LETTER(*(p+1))-1][1];
				k=0;
			}
			else if(!existent_roots[LETTER(*(p+1))-1]){
				k=0;
				row=0;
			}
			break;
		case 1:
			if(existent_inflexions[LETTER(*(p+1))-1]){
				depth = depth + wordsinflected_each_root[LETTER(*(p+1))-1][1];
				row = wordsinflected_each_root[LETTER(*(p+1))-1][1];
				k=0;
			}
			else if(!existent_inflexions[LETTER(*(p+1))-1]){
				k=0;
				row=0;
			}
			break;
	}
		
	char temp[depth][HMAX];
	short int temp_each_len[depth], sortresult;
	for(int i=0;i<depth;i++){
		temp_each_len[i]=0;
		for(int j=0;j<HMAX;j++){
			temp[i][j]='#';
		}
	}
	storage=&temp[0][0];
	if(row!=0) fileoperations(*(p+1),inflected,&filestatus,"load",&temp[0][0],temp_each_len);
	while(k<recv.length()){
		
		int singleton=0;
		while(*(p+k+1)!='|'){
			*(storage+row*HMAX+singleton)=*(p+k+1);
			temp_each_len[row]=temp_each_len[row]+1;
			singleton=singleton+1;
			k=k+1;
		}
		k=k+1;
		if(k==recv.length()-1) break;
		row=row+1;
		
	}	
	sortresult=sort(storage,temp_each_len,depth,0);
	for(int i=0;i<depth;i++){
		if(temp_each_len[i]!=0){
			temp[i][temp_each_len[i]]=SEPERATOR;
			temp_each_len[i]=temp_each_len[i]+1;
		    pushsize=pushsize+temp_each_len[i];
		}
	}
	pushsize=pushsize+1;
	char pushregion[pushsize], *pregion;
	pregion=pushregion;
	*(pregion+0)=SEPERATOR;
	pregion=pregion+1;
	for(int i=0;i<depth;i++){
		if(temp_each_len[i]!=0){
			memmove((char *)pregion,(char *)storage+i*HMAX+0,temp_each_len[i]*sizeof(char));
			pregion=pregion+temp_each_len[i];
		}
	}
	pregion=pushregion;

	ofstream writer(fname.c_str(), ios::out);
	if(writer.is_open()) writer.write((char *)&pushregion, sizeof(char)*pushsize);
	else return 0;
	writer.close();
	
	if(inflected){
		updated_inflexion[(int)LETTER(root)-1] = (bool)1;
		master_updated = true;
	}
	else{
		updated_root[(int)LETTER(root)-1] = (bool)1;
		master_updated = true;
	}
	
	return 1;
}


int Words::create_stringsbyroot(string s1root, string s2related){
	
	int each_rootcounts[26][2]={0};
	int each_relatedcounts[26][2]={0};
	int s1root_tot_letters=0, s2related_tot_letters=0;
	if(s1root.empty() && s2related.empty()) return 0;
	
	for(short int j=1;j<=2;j++){
		if(j==1){
			const char *p=s1root.data();
			for(int i=0;i<s1root.length();i++){
				if(*(p+i)=='|' && i!=s1root.length()-1){
					each_rootcounts[LETTER(*(p+i+1))-1][0]=LETTER(*(p+i+1));
					each_rootcounts[LETTER(*(p+i+1))-1][1]=each_rootcounts[LETTER(*(p+i+1))-1][1]+1;
				}
			}
		}
		else{
			const char *p=s2related.data();
			for(int i=0;i<s2related.length();i++){
				if(*(p+i)=='|' && i!=s2related.length()-1){
					each_relatedcounts[LETTER(*(p+i+1))-1][0]=LETTER(*(p+i+1));
					each_relatedcounts[LETTER(*(p+i+1))-1][1]=each_relatedcounts[LETTER(*(p+i+1))-1][1]+1;
				}
			}
		}
	}	
	for(short int i=0;i<26;i++){
		if(each_rootcounts[i][0]!=0) s1root_tot_letters=s1root_tot_letters+1;
		if(each_relatedcounts[i][0]!=0) s2related_tot_letters=s2related_tot_letters+1;
	}
	
	string words[s1root_tot_letters], words_inflected[s2related_tot_letters];
	const char *p;
	int length, size, written_roots[s1root_tot_letters],written_related[s2related_tot_letters];
	string *strptr;
	for(short int i=1;i<=2;i++){
		switch(i){
			case 1:
				p=s1root.data();
				length=s1root.length();
				strptr=words;
				size=s1root_tot_letters;
				break;
			case 2:
				p=s2related.data();
				length=s2related.length();
				strptr=words_inflected;
				size=s2related_tot_letters;
				break;
		}
		for(int j=1;j<length;j++){
				string temp="|";
				while(*(p+j)!='|'){
					temp=temp+*(p+j);
					j=j+1;
				}
				int k=0;
				while(k<size){
					if(!(strptr+k)->empty()){ //object pointer
						const char *q=(strptr+k)->data();
						if(*(q+1)==*(p+j-temp.length()+1)){
							*(strptr+k)=*(strptr+k)+temp;
							break;
						}
					}
					else if((strptr+k)->empty()){
						*(strptr+k)=*(strptr+k)+temp;
						break;
					}
					k=k+1;
				}
		}
		for(int z=0;z<size;z++){
		 *(strptr+z)=*(strptr+z)+"|";
		}
	}
	
	bool status_words[s1root_tot_letters], status_words_inflected[s2related_tot_letters];
	for(int i=0;i<s1root_tot_letters;i++) status_words[i] = (bool)0;
	for(int i=0;i<s2related_tot_letters;i++) status_words_inflected[i] = (bool)0;
	
	if(s1root_tot_letters!=0){
		for(int i=0;i<s1root_tot_letters;i++) status_words[i] = (bool)updatedata_words(words[i]);
	}
	if(s2related_tot_letters!=0){
		for(int i=0;i<s2related_tot_letters;i++) status_words_inflected[i] = (bool)updatedata_words(words_inflected[i],1);
	}

	//HERE ADDITION TO CHECK ALL ELEMENTS IN STATUS_WORDS ARE 1(return from updatedata) else another mechanism. wont be needed it seems
	//updatedata_words should do the file error bits and return the final outcome here.
	return 1;
}


int Words::passdata(istream &input,ostream &output){
	
	int b = 0, count = 0, temp, flow=1; //here only cin and cout is acceptable
	char type='@';
	string bufstring, words, words_inflected, msg1, msg2, info;
	words="|";
	words_inflected="|";
	msg1="Set to type: words\nStart entering words...\n";
	msg2="Set to type: related-words\nStart entering words...\n";
	info="Add words seperated by (space) or (tab)\nTo switch between words and related words. Enter (#)-words, ($)-related words at (>) prompt.\nEnter type of word: ";
	output<<info;
	while(1){
		while((b=input.get())!=(int)'\n'){ //can input cntrl+D at the beginning when function is called meaning no input to give
			if(b==std::istream::traits_type::eof()) break;
			if(b==(int)'$') type='$';
			else if(b==(int)'#') type='#';
		}
		if(type=='$' || type=='#') break;
		else if(b==std::istream::traits_type::eof()) return 0;
	}
	if(type=='#') output<<msg1;
	else if(type=='$') output<<msg2;

	//input.clear();
	while( (b = input.get()) != std::istream::traits_type::eof() ) {
		std::stringstream stream,stream_inflected;
		if(!stream_permissible_set(b)){
			flow=0;
			if(b==(int)'$'){
				type='$';
				std::cout<<msg2;
			}
			else if(b==(int)'#'){
				type='#';
				std::cout<<msg1;
			}
			temp=input.get();
		}
		if(stream_permissible_set(b)==1 && stream_permissible_set((char)input.peek())==0){
			flow=0;
			bufstring.erase(0,bufstring.length());
		}
		else if(stream_permissible_set(b)==1 && stream_permissible_set((char)input.peek())==1){
			flow=1;
			}
		else if(stream_permissible_set(b)==1 && stream_permissible_set((char)input.peek())==-1){
			flow=1;
		}		
		if(stream_permissible_set(b)==1){
			 if(flow==1) bufstring=bufstring+(char)b;
		 }			 
		if(stream_permissible_set(b)==1 && stream_permissible_set((char)input.peek())==-1){
			bufstring=bufstring+'|';
			if(type=='#') stream<<bufstring;
			else if(type=='$') stream_inflected<<bufstring;
			bufstring.erase(0,bufstring.length());
		}

		words = words+stream.str();
		words_inflected = words_inflected+stream_inflected.str();
		std::cout<<">";
		std::cout<<"\b";
	}


	std::cout<<"#:"<<words<<"\n";
	std::cout<<"$:"<<words_inflected<<"\n";
	input.clear(); //eof bit is cleared
	output.clear(); //eof bit is cleared
  
	if(words.length()==1 && words_inflected.length()==1) return 0;
	else{
	  create_stringsbyroot(words,words_inflected);
	  return 1;
	}
}


void Words::create_object(const Words &object){
	username=object.username;
	init_word_counts=object.init_word_counts;
	for(int i=0;i<DEPTH;i++){
		for(int j=0;j<HMAX;j++){	
			init_word[i][j]=object.init_word[i][j];
		}
	}
	for(int i=0;i<DEPTH;i++) init_each_len[i]=object.init_each_len[i];
}


void Words::AssignUser_to_object(string name){
	this->username=name;
}


void Words::initiater_wordcounts_onfile(){
	//has to be modified , file status need be checked and then put existent roots
	//filestatus can be static
	short int filestatus_root[26]; //0 means success
	short int filestatus_related[26];
	short int root_numeric;
	if((char)97=='a') root_numeric=97;
	for(short int i=0;i<26;i++){
		filestatus_root[i]=-1;
		filestatus_related[i]=-1;
		existent_roots[i]=(bool)fileoperations((char)(root_numeric+i),0,&filestatus_root[i]);
		existent_inflexions[i]=(bool)fileoperations((char)(root_numeric+i),1,&filestatus_related[i]);
	}
	for(short int i=0;i<26;i++) cout<<"Existent_root:"<<i<<"-->"<<existent_roots[i]<<" "<<"Existent_related:"<<i<<"-->"<<existent_inflexions[i]<<endl;
}

Words::Words(){
	
	cout<<"\nConstructing Base object : "<<this->username<<endl;
	
	short int i,j;
	username="user_unassigned";
	
	init_word_counts=0;
	vocab_word_counts=0;
	term_word_counts=0;
	
	reset_array(&vocab_word[0][0],sizeof vocab_word);
	reset_array(&init_word[0][0],sizeof init_word);
	reset_array(&term_word[0][0],sizeof term_word);
	
	reset_array(&vocab_each_len[0],sizeof vocab_each_len);
	reset_array(&init_each_len[0],sizeof init_each_len);
	reset_array(&term_each_len[0],sizeof init_each_len);
	
	reset_array(&vocab_lower_indices[0][0],sizeof vocab_lower_indices,0,-1);
	reset_array(&init_lower_indices[0][0],sizeof init_lower_indices,0,-1);
	reset_array(&term_lower_indices[0][0],sizeof term_lower_indices,0,-1);
	
	
	reset_array(&words_each_root[0][0],sizeof words_each_root);
	reset_array(&wordsinflected_each_root[0][0],sizeof wordsinflected_each_root);
	//bools are auto false
	for(i=0;i<26;i++){
		existent_roots[i]=(bool)0;
		existent_inflexions[i]=(bool)0;
		updated_root[i] = (bool)0;
		updated_inflexion[i] = (bool)0;
	}

	master_updated = false;

	initiater_wordcounts_onfile();
	Words::totalobjects = Words::totalobjects+1;
}

Words::~Words(){
	
	cout<<"\nUser exiting :"<<this->username<<" ";
	totalobjects=totalobjects-1;
	cout<<"Total remaining objects:"<<totalobjects<<endl;
}

void Words::show_usernodeinfo(){
	
	short int i,j;
	cout<<"USERNODE INFO:"<<endl;
	cout<<"Username : "<<username<<"\n";
	
	cout<<"Vocab word :"<<endl;
	show_array(&vocab_word[0][0],sizeof vocab_word,HMAX);
	cout<<"Init word :"<<endl;
	show_array(&init_word[0][0],sizeof init_word,HMAX);
	cout<<"Term word :"<<endl;
	show_array(&term_word[0][0],sizeof term_word,HMAX);
	
	cout<<"Vocab indices :"<<endl;
	show_array(&vocab_lower_indices[0][0],sizeof vocab_lower_indices,2);
	cout<<"Init indices :"<<endl;
	show_array(&init_lower_indices[0][0],sizeof init_lower_indices,2);
	cout<<"Term indices :"<<endl;
	show_array(&term_lower_indices[0][0],sizeof term_lower_indices,2);

	cout<<"ON FILE word counts :"<<endl;
	for(i=0;i<26;i++){
		if(existent_roots[i]) cout<<words_each_root[i][0]<<" : "<<words_each_root[i][1]<<endl;
	}
	
	cout<<"Total objects in existence:"<<totalobjects<<endl;
	
	size_t totalsizeofobject = sizeof username + sizeof existent_roots + sizeof existent_inflexions + sizeof words_each_root
	+ sizeof wordsinflected_each_root + sizeof totalobjects + sizeof vocab_word + sizeof init_word + sizeof term_word
	+ sizeof vocab_each_len + sizeof init_each_len + sizeof term_each_len + sizeof vocab_word_counts + sizeof init_word_counts
	+ sizeof term_word_counts + sizeof vocab_lower_indices + sizeof term_lower_indices + sizeof init_lower_indices;
	cout<<"TOTOBJECTSIZE: "<<totalsizeofobject<<"--Kb: "<<totalsizeofobject/1024<<endl;
}


//---------------------------------------------

Words *Gptr[10],*buffptr;
class QueryResolver: public Words{

	#define SELECTEDMAX 2500
	#define HMAX 40
	#define AZ 26
	#define REXT "_root.txt"
	#define IEXT "_inflected.txt"
	#define FILLER '#'
	#define SEPERATOR '|'
	#define INIT '_'
	#define TRAN '+'
	#define TERM '~'
	#define VOC '.'
	#define INFTREE "_inflectedTree"
	#define INFDATA "_inflectedTreeData"
	#define ROOTTREE "_rootTree"
	#define ROOTDATA "_rootTreeData"
	
	private:
	
	bool status_troot[AZ];
	bool status_tinflected[AZ];
	
	bool fexist_tree_root[AZ];
	bool fexist_tree_inf[AZ];
	bool fexist_data_root[AZ];
	bool fexist_data_inf[AZ];

	int root_filesize[AZ][2];
	int inflexion_filesize[AZ][2];
	
	int maxofwords;
	short int hmaxofall;
	char *selected;
	int listmarker;
	char matchword[1][HMAX];
	char shortlisted[50][HMAX]; //not to be used
	
	
	char lastseek_root;
	char lastseek_inflexion;
	
	struct userdata {
		
		char *node_root;
		char *node_inflexion;
		short int *node_root_eachlen;
		short int *node_inflexion_eachlen;
		
		int node_root_wordcounts;
		int node_inflexion_wordcounts;
		
		int *indices_root;
		int totindices_root;
		int *indices_inflexion;
		int totindices_inflexion;
		
		char letter_onroot;
		char letter_oninflexion;
		char index_for;
		enum indexpos{second,last} pos;  //values are 0 and 1 integers
		enum nodetype {root,inflexion} ntype;
	} info;

	struct treedata {
		
		char letter;
		bool first[26];
		short int firstnodes[26];
		char childtable[26][26];
		short int rows_child[26][26];
		short int hmax_child[26][26];
		
	} root[26],inf[26],*tr;

	struct trees{
		
		char **child[26];
		char *bitable;
		bool allocated;
		bool populated;
		
	} root_tree[26], inf_tree[26], *trmap;
	
	
	private:
	
	char numberedletter(int num);
	int allocatenode(bool ntype, int wordcounts, int xymultiplier=HMAX);
	int populatenode(int num_nodeletter, bool ntype);
	int setindexinfo(char index_for, string pos, string ntype, char nodeletter=FILLER);
	int second_last_letterindexer(struct userdata *sptr);
	int loadfilesize(char letter, bool inflexion=0);
	void initiateinfo();
	void structinfo();
	string givestring(const char *storage, int atrow, short int length, short int hlen=HMAX);
	string makestring(const char *storage, int atrow, short int hlen);
	int bit_search(string pattern, string text);
	int findPattern(string pattern, string text);
	int queryrun(string qsegment, string ntype);
	int query(string qstr);
	int singlevoc(string qsegment, string ntype);
	void free_userdata();
	short int treeIndexing(char *source, int rows, short int inpos, char inletter, short int rowindex, char rootletter, bool inflexion, short int bi);
	short int maxLen(short int *lensource, int counts);
	void initializeTree(char letter, bool inflexion);
	int allocateTree(char letter, bool inflexion);
	int populateTree(char letter, bool inflexion, char *bigrid);
	int createTree(char letter, bool inflexion);
	int loadTree(char letter, bool inflexion);
	short int freeTree(char letter, bool inflexion);
	int loadTreePopulation(char letter, bool inflexion);
	bool file_treesExists(const char *fname);
	int loadingforQuery(char letter);
	int fourth_indexer(char *storage, signed short int *indexer_ptr, short int counts, short int hlen, short int fourth_ind = -1);
	int refillSelected(int tmaxofwords, short int thmaxofall);

	public:
	
	QueryResolver();
	~QueryResolver();
	void treeDataInfo(char letter, bool inflexion);
	void showTree(char letter, bool inflexion);
	void showuserdata();
	void showmaxtypes();
	int treeQuery(string qsegment);
	void showmword();
	
	
} Qobject[1];


char QueryResolver::numberedletter(int num){
	char x = 'a';
	if(1 <= num && num <= AZ) return (char)(num+(int)x-1);
	else return (char)0; //will return null (NUL)
	//function tested
	// a=97 z=122 so (1+97-1 = 97 is a) (26+97-1 = 122 is z)
}


int QueryResolver::allocatenode(bool ntype, int wordcounts, int xymultiplier){
	//returns -1,0 and 1(normal), 0(allocation did not happen). Check status[3] here
	
	int status[3] = {-1,-1,-1};
	char letter;
	char *node;
	short int *eachlen;
	int *indices;
	
	switch((int)ntype){
		case 0:
			letter = info.letter_onroot;
			node = info.node_root;
			eachlen = info.node_root_eachlen;
			indices = info.indices_root;
			break;
		case 1:
			letter = info.letter_oninflexion;
			node = info.node_inflexion;
			eachlen = info.node_inflexion_eachlen;
			indices = info.indices_inflexion;
			break;
		default:
			return -1;
	}
	if(wordcounts < 1) return -1;
	if(xymultiplier < 1) return -1;
	
	switch(letter){
		case FILLER:
			node = (char *)dynarr_alloc(wordcounts*xymultiplier,sizeof(char));
			if(!node) status[0] = 0;
			else status[0] = 1;
			
			eachlen = (short int *)dynarr_alloc(wordcounts,sizeof(short int));
			if(!eachlen) status[1] = 0;
			else status[1] = 1;
			
			indices = (int *)dynarr_alloc(wordcounts,sizeof(int));
			if(!indices) status[2] = 0;
			else status[2] = 1;
			
			break;
			
		default:
			if(node){
				if(wordcounts*xymultiplier > dynarr_elements((char *)node)){
					node = (char *)dynarr_resize((char *)node, wordcounts*xymultiplier);
					if(!dynarr_status((char *)node)) status[0] = 0;
					else status[0] = 1;
				}
				else status[0] = 1;
			}
			else status[0] = -1;
			
			if(eachlen){
				if(wordcounts > dynarr_elements((short int *)eachlen)){
					eachlen = (short int *)dynarr_resize((short int *)eachlen, wordcounts);
					if(!dynarr_status((short int *)eachlen)) status[1] = 0;
					else status[1] = 1;
				}
				else status[1] = 1;
			}
			else status[1] = -1;
			
			if(indices){
				if(wordcounts > dynarr_elements((int *)indices)){
					indices = (int *)dynarr_resize((int *)indices, wordcounts);
					if(!dynarr_status((int *)indices)) status[2] = 0;
					else status[2] = 1;
				}
				else status[2] = 1;
			}
			else status[2] = -1;
	}
	
	if(status[0]==1 && status[1]==1 && status[2]==1){
		reset_array((char *)node, dynarr_size((char *)node));
		reset_array((short int *)eachlen, dynarr_size((short int *)eachlen));
		reset_array((int *)indices, dynarr_size((int *)indices));
		
		switch((int)ntype){
			case 0:
				info.node_root = node;
				info.node_root_eachlen = eachlen;
				info.indices_root = indices;
				break;
			case 1:
				info.node_inflexion = node;
				info.node_inflexion_eachlen = eachlen;
				info.indices_inflexion = indices;
				break;
		}
		return 1;
	}
	else return 0;
}


int QueryResolver::populatenode(int num_nodeletter, bool ntype){
	//fstatus is 1,2,3, 1 means eof , 2 means cannot open, 3 means fatal error
	
	short int fstatus;
	char letter;
	letter = numberedletter(num_nodeletter);
	if(permissible_set(letter) && letter==SEPERATOR) return 0;
	
	if(ntype==0){
		if(fileoperations(letter, info.ntype, &fstatus, "load", info.node_root, info.node_root_eachlen)==1) return 1;
		else return (int)(-fstatus);
	}
	else if(ntype==1){
		if(fileoperations(letter, info.ntype, &fstatus, "load", info.node_inflexion, info.node_inflexion_eachlen)==1) return 1;
		else return (int)(-fstatus);
	}
	else return 0;
	//return VALUES  as -1,-2,-3 or 1(normal) and 0
}


int QueryResolver::setindexinfo(char index_for, string pos, string ntype, char nodeletter){
	
	//returns 0 or 1 (normal termination) else -1,-2,-3 for filestatus and -4 for noallocation, -5 for nonexistent
	if(permissible_set(index_for) != 1 && index_for != FILLER) return 0;
	
	if(pos == "second") info.pos = userdata::second;
	else if(pos == "last") info.pos = userdata::last;
	else return 0;
	
	if(ntype == "root") info.ntype = userdata::root;
	else if(ntype == "inflexion") info.ntype = userdata::inflexion;
	else return 0;
	
	if(nodeletter == FILLER){
		info.index_for = index_for;
		return 1; //required
	}
	else if(permissible_set(nodeletter) && nodeletter==SEPERATOR) return 0; //IF THIS IS OK, REST FOLLOWS
	cout<<"iterate setindexinfo for allocate"<<endl;
	bool *updated;
	bool *existent;
	int *wordcounts;
	int *node_wordcounts;
	char *letteron;
	short int filestatus;
	int allocstatus;
	int num_nodeletter;
	int constant;
	
	switch(info.ntype){
		case 0:
			updated = &updated_root[0];
			existent = &existent_roots[0];
			wordcounts = &words_each_root[0][0];
			node_wordcounts = &info.node_root_wordcounts;
			letteron = &info.letter_onroot;
			break;
			
		case 1:
			updated = &updated_inflexion[0];
			existent = &existent_inflexions[0];
			wordcounts = &wordsinflected_each_root[0][0];
			node_wordcounts = &info.node_inflexion_wordcounts;
			letteron = &info.letter_oninflexion;
			break;
	}
	
	num_nodeletter = LETTER(nodeletter);
	constant = 2*(num_nodeletter-1);
	
	if(!*(updated + num_nodeletter-1)){
		if(*(existent + num_nodeletter-1)){
			allocstatus = allocatenode(info.ntype, *(wordcounts + constant +1));
			if(allocstatus == 1){
				filestatus = (short int)populatenode(num_nodeletter, info.ntype);
				if(filestatus != 1) return (int)filestatus;
				else{
					*node_wordcounts = *(wordcounts + constant +1);
					*letteron = nodeletter;
					info.index_for = index_for;
				}
			}
			else return -4;
		}
		else return -5;
	}
	else if(*(updated + num_nodeletter-1)){
		fileoperations(nodeletter, info.ntype, &filestatus);
		if(filestatus != 0) return (int)(-filestatus);
		else{
			*(existent + num_nodeletter-1) = (bool)1;
			*(updated + num_nodeletter-1) = (bool)0; //updated set again to zero as now read
		}
		
		allocstatus = allocatenode(info.ntype, *(wordcounts + constant +1));
		if(allocstatus == 1){
			filestatus = (short int)populatenode(num_nodeletter, info.ntype);
			if(filestatus != 1) return (int)filestatus;
			else{
				*node_wordcounts = *(wordcounts + constant +1);
				*letteron = nodeletter;
				info.index_for = index_for;
			}
		}
		else return -4;
	}
	
	
	return 1;
}


int QueryResolver::second_last_letterindexer(struct userdata *sptr){
	
	//ASSUMPTION: default is HMAX
	//with only 1 letter as the word, the filler on next element does not get matched. ok with the code
	
	const char *source_array;
	int j,k,delta,*deltaptr; 
	int numberedch,numofletter;
	int rowcounts,row;
	int *indices;
	short int *eachlen;
	short int defint;
	
	defint=2;
	delta = 0;
	k=0;
	eachlen=&defint;
	
	if(sptr->ntype == userdata::inflexion){
		if(sptr->node_inflexion_wordcounts<1) return 0;
		else if(sptr->pos == userdata::last){
			deltaptr = &row;
			eachlen = sptr->node_inflexion_eachlen;
		}
		else deltaptr = &delta;
		
		rowcounts = sptr->node_inflexion_wordcounts;
		indices = sptr->indices_inflexion;
		source_array = sptr->node_inflexion;
	}
	else if(sptr->ntype == userdata::root){
		if(sptr->node_root_wordcounts<1) return 0;
		else if(sptr->pos == userdata::last){
			deltaptr=&row;
			eachlen = sptr->node_root_eachlen;
		}
		else deltaptr = &delta;
		
		rowcounts = sptr->node_root_wordcounts;
		indices = sptr->indices_root;
		source_array = sptr->node_root;
	}
	
	numofletter = (int)LETTER(sptr->index_for);
	
	for(row=0;row<rowcounts;row++){
		j=*(eachlen+*deltaptr);
		numberedch = (int)(LETTER(*(source_array+row*HMAX+j-1)));
		if(numberedch == numofletter){
			*(indices+k)=row;
			k=k+1;
		}
	}
	if(sptr->ntype == userdata::root) sptr->totindices_root = k;
	else sptr->totindices_inflexion = k;
	
	return 1;
}


int QueryResolver::loadfilesize(char letter, bool inflexion){
	
	string root,ext,filename;
	root = letter;
	ext = inflexion!=1 ? REXT : IEXT;

	if(permissible_set(letter) && letter!=SEPERATOR) filename = root+ext;
	else return 0;
	
	FILE *fp;
	if((fp = fopen(filename.c_str(),"r+")) == NULL) return 0; //text book code or REFER SNIPPET0 finalize the mode as rb or not
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp); //<-use long
	fseek(fp, 0, SEEK_SET); //this may not require but use. even if i change the value to say 22, it still gives correct size.
	fclose(fp);

	if(inflexion){
		inflexion_filesize[LETTER(letter)-1][0] = LETTER(letter);
		inflexion_filesize[LETTER(letter)-1][1] = (int)fsize;
	}
	else{
		root_filesize[LETTER(letter)-1][0] = LETTER(letter);
		root_filesize[LETTER(letter)-1][1] = (int)fsize;
	}
	return 1;
}


void QueryResolver::initiateinfo(){
	
	info.letter_onroot = FILLER;
	info.letter_oninflexion = FILLER;
	info.index_for = FILLER;
	info.totindices_root = 0;
	info.totindices_inflexion=0;
	
	info.node_root = 0;
	info.node_root_eachlen = 0;
	info.node_root_wordcounts = 0;
	info.indices_root = 0;
	
	info.node_inflexion = 0;
	info.node_inflexion_eachlen = 0;
	info.node_inflexion_wordcounts = 0;
	info.indices_inflexion = 0;
}



string QueryResolver::givestring(const char *storage, int atrow, short int length, short int hlen){
	
	string word;
	for(short int i=0;i<length;i++){
		word = word + *(storage+atrow*hlen+i);
	}
	
	return word;
}

string QueryResolver::makestring(const char *storage, int atrow, short int hlen){
	
	string word;
	for(short int i=0;i<hlen;i++){
		if(permissible_set(*(storage+atrow*hlen+i)) == 1){
			word = word + *(storage+atrow*hlen+i);
		}
		else break;
	}
	
	return word;
}

int QueryResolver::bit_search(string pattern, string text){
	
	int m = pattern.length();
	long pattern_mask[256];
	//Initialize the bit array R
	long R = ~1;
	if (m == 0)	return -1;
	if (m > 63)	return -1; //Pattern is too long;
	//Initialize the pattern bitmasks
	for(int i=0; i <= 255; ++i)	pattern_mask[i] = ~0;
	for(int i=0; i < m; ++i)	pattern_mask[pattern[i]] &= ~(1L << i);
	for(int i=0; i < text.length(); ++i){
		//Update the bit array
		R |= pattern_mask[text[i]];
		R <<= 1;
		if((R & (1L << m)) == 0)	return i-m+1;
	}
	return -1;
}

int QueryResolver::findPattern(string pattern, string text){
	
	if(pattern.length() > text.length()) return -1;
	int pos = bit_search(pattern, text);
	return pos;
}

int QueryResolver::queryrun(string qsegment,string ntype){ //alternate approach
	
	int cmpvalue, lendiff;
	int i, counts;
	int *indpos;
	short int *eachlen;
	short int hlen;
	char *dest;
	char *source;
	string currentword;
	
	hlen = HMAX;
	dest = &shortlisted[0][0];
	
	if(info.ntype == userdata::root){
		source = info.node_root;
		eachlen = info.node_root_eachlen;
	}
	else{
		source = info.node_inflexion;
		eachlen = info.node_inflexion_eachlen;
	}

	if(info.ntype == userdata::root && info.index_for == FILLER) counts = info.node_root_wordcounts;
	else if(info.ntype == userdata::root && info.index_for != FILLER){
		counts = info.totindices_root;
		indpos = info.indices_root;
	}
	
	if(info.ntype == userdata::inflexion && info.index_for == FILLER) counts = info.node_inflexion_wordcounts;
	else if(info.ntype == userdata::inflexion && info.index_for != FILLER){
		counts = info.totindices_inflexion;
		indpos = info.indices_inflexion;
	}
	
	
	switch(qsegment[0]){
		case TERM:
			qsegment.erase(0,1);
			for(i=0;i<counts;i++){
				currentword = givestring(source,*(indpos+i),*(eachlen+*(indpos+i)));
				cmpvalue = findPattern(qsegment,currentword);
				lendiff = currentword.length()-qsegment.length();
				if(cmpvalue == lendiff && cmpvalue > -1){
						memcpy((char *)(dest+listmarker*hlen+0), (char *)(source+*(indpos+i)*hlen+0),sizeof(char)*hlen);
						listmarker = listmarker + 1;
				}
			}
			break;
			
		case TRAN:
			qsegment.erase(0,1);
			for(i=0;i<counts;i++){
				currentword = givestring(source,i,*(eachlen+i));
				cmpvalue = findPattern(qsegment,currentword);
				if(cmpvalue != -1){
					memcpy((char *)(dest+listmarker*hlen+0), (char *)(source+i*hlen+0),sizeof(char)*hlen);
					listmarker = listmarker + 1;
				}
				if(listmarker > 1000) break;
			}
			break;
		
		case VOC:
			qsegment.erase(0,1);
			for(i=0;i<counts;i++){
				currentword = givestring(source,*(indpos+i),*(eachlen+*(indpos+i)));
				cmpvalue = findPattern(qsegment,currentword);
				if(cmpvalue != -1){
					if(currentword.length() == qsegment.length()){
						reset_array(&matchword[0][0], sizeof matchword);
						memcpy(&matchword[0][0], (char *)(source+*(indpos+i)*hlen+0), sizeof(char)*hlen);
						return 1;
					}
				}
			}
			return 0;
			break;
			
		case INIT:
			qsegment.erase(0,1);
			for(i=0;i<counts;i++){
				currentword = givestring(source,*(indpos+i),*(eachlen+*(indpos+i)));
				cmpvalue = findPattern(qsegment,currentword);
				lendiff = currentword.length()-qsegment.length();
				if(cmpvalue == 0){
						memcpy((char *)(dest+listmarker*hlen+0), (char *)(source+*(indpos+i)*hlen+0),sizeof(char)*hlen);
						listmarker = listmarker + 1;
				}
			}
			break;
			
		default:
			return 0;
	}

	//listmarker = 0;
	return 1;
}

int QueryResolver::query(string qsegment){ //alternate approach
	// example ~ed
	const char *str = qsegment.data();
	char indexfor;
	int fnout;
	string last="last";
	string second="second";
	string root="root";
	string inflexion="inflexion";
	
	
	if(qsegment[0] != INIT && qsegment[0] != TRAN && qsegment[0] != TERM && qsegment[0] != VOC) return 0;
	reset_array(&shortlisted[0][0], sizeof shortlisted);
	
	switch(qsegment[0]){
		
		case TERM:
			listmarker = 0;
			indexfor = *(str+qsegment.length()-1);
			for(int i=1;i<=AZ;i++){
				if(existent_roots[i-1] || updated_root[i-1]){
					setindexinfo(indexfor,last,root,numberedletter(i));
					if(info.node_root){
						if(*(info.node_root+0) == numberedletter(i)){
							second_last_letterindexer(&info);
							queryrun(qsegment,root);
						}
					}
				}
				if(existent_inflexions[i-1] || updated_inflexion[i-1]){
					setindexinfo(indexfor,last,inflexion,numberedletter(i));
					if(info.node_inflexion){
						if(*(info.node_inflexion+0) == numberedletter(i)){
							second_last_letterindexer(&info);
							queryrun(qsegment,inflexion);
						}
					}
				}
			}
			
			break;
			
		case TRAN: //only copied section to alter
			listmarker = 0;
			indexfor = FILLER;
			for(int i=1;i<=AZ;i++){
				if(existent_roots[i-1] || updated_root[i-1]){
					setindexinfo(indexfor,last,root,numberedletter(i));
					if(info.node_root){
						if(*(info.node_root+0) == numberedletter(i)){
							queryrun(qsegment,root);
						}
					}
				}
				if(existent_inflexions[i-1] || updated_inflexion[i-1]){
					setindexinfo(indexfor,last,inflexion,numberedletter(i));
					if(info.node_inflexion){
						if(*(info.node_inflexion+0) == numberedletter(i)){
							queryrun(qsegment,inflexion);
						}
					}
				}
			}
			
			break;
			
		case VOC:
			
			if(existent_roots[LETTER(qsegment[1])-1] || updated_root[LETTER(qsegment[1])-1]){
				
				if(lastseek_root == qsegment[1] && updated_root[LETTER(qsegment[1])-1]==(bool)0){
					
					if(qsegment.length() == 2) return singlevoc(qsegment,root);
					else indexfor = *(str+2);
					fnout = setindexinfo(indexfor,second,root);
				}
				else{
					
					if(qsegment.length() == 2)	indexfor = FILLER;
					else indexfor = *(str+2);
					fnout = setindexinfo(indexfor, second, root, qsegment[1]);
				}
				
				if(fnout == 1){
					if(info.node_root){
						if(*(info.node_root+0) == qsegment[1]){
							
							if(qsegment.length() == 2) return singlevoc(qsegment,root);
							
							second_last_letterindexer(&info);
							if(queryrun(qsegment,root) == 1){
								lastseek_root = qsegment[1];
								return 1;
							}
						}
					}
				}
				else return fnout;
				
				lastseek_root = qsegment[1];
			}
			
			if(existent_inflexions[LETTER(qsegment[1])-1] || updated_inflexion[LETTER(qsegment[1])-1]){
				
				if(lastseek_inflexion == qsegment[1] && updated_inflexion[LETTER(qsegment[1])-1]==(bool)0){
					
					if(qsegment.length() == 2) return singlevoc(qsegment,inflexion);
					else indexfor = *(str+2);
					fnout = setindexinfo(indexfor, second, inflexion);
				}
				else{
					
					if(qsegment.length() == 2)	indexfor = FILLER;
					else indexfor = *(str+2);
					fnout = setindexinfo(indexfor, second, inflexion, qsegment[1]);
				}
				
				if(fnout == 1){
					if(info.node_inflexion){
						if(*(info.node_inflexion+0) == qsegment[1]){
							
							if(qsegment.length() == 2) return singlevoc(qsegment, inflexion);
							
							second_last_letterindexer(&info);
							if(queryrun(qsegment,inflexion) == 1){
								lastseek_inflexion = qsegment[1];
								return 1;
							}
						}
					}
				}
				else return fnout;
				
				lastseek_inflexion = qsegment[1];
			}
			
			return 0;
			
			break;
			
		case INIT:
			listmarker = 0;
			indexfor = *(str+2);
			if(existent_roots[LETTER(qsegment[1])-1] || updated_root[LETTER(qsegment[1])-1]){
				setindexinfo(indexfor,second,root,qsegment[1]);
				if(info.node_root){
					if(*(info.node_root+0) == qsegment[1]){
						second_last_letterindexer(&info);
						queryrun(qsegment,root);
					}
				}
			}
			if(existent_inflexions[LETTER(qsegment[1])-1] || updated_inflexion[LETTER(qsegment[1])-1]){
				setindexinfo(indexfor,second,inflexion,qsegment[1]);
				if(info.node_inflexion){
					if(*(info.node_inflexion+0) == qsegment[1]){
						second_last_letterindexer(&info);
						queryrun(qsegment,inflexion);
					}
				}
			}
			
			break;
	}
	
	return 1;
}

int QueryResolver::singlevoc(string qsegment, string ntype){ //alternate approach
	
	string currentword;
	string root = "root";
	string inflexion = "inflexion";
	short int hlen = HMAX;
	qsegment.erase(0,1);
	
	if(ntype == root){
		
		currentword = givestring(info.node_root,0,*(info.node_root_eachlen+0));
		if(findPattern(qsegment,currentword) == 0 && currentword.length() == qsegment.length()){
			
			reset_array(&matchword[0][0], sizeof matchword);
			memcpy(&matchword[0][0], (char *)(info.node_root+0*hlen), sizeof(char)*hlen);
			lastseek_root = qsegment[0];
			return 1;
			
		}
		else{
			lastseek_root = qsegment[0];
			return 0;
		}
	}
	else if(ntype == inflexion){
		
		currentword = givestring(info.node_inflexion,0,*(info.node_inflexion_eachlen+0));
		if(findPattern(qsegment,currentword) == 0 && currentword.length() == qsegment.length()){
			
				reset_array(&matchword[0][0], sizeof matchword);
				memcpy(&matchword[0][0], (char *)(info.node_inflexion+0*hlen), sizeof(char)*hlen);
				lastseek_inflexion = qsegment[0];
				return 1;
			
		}
		else{
			lastseek_inflexion = qsegment[0];
			return 0;
		}
	}
	else return -1;
}


void QueryResolver::initializeTree(char letter, bool inflexion){
	
	short int numofletter = LETTER(letter)-1;
	switch((int)inflexion){
		case 0:
			tr = &root[numofletter];
			trmap = &root_tree[numofletter];
			break;
		case 1:
			tr = &inf[numofletter];
			trmap = &inf_tree[numofletter];
			break;
	}
	
	this->tr->letter = FILLER;
	for(short int i=0;i<AZ;i++)	tr->first[i] = (bool)0;
	reset_array(&(tr->firstnodes[0]), sizeof tr->firstnodes);
	reset_array(&(tr->childtable[0][0]), sizeof tr->childtable);
	reset_array(&(tr->rows_child[0][0]), sizeof tr->rows_child, 26*26,-1);
	reset_array(&(tr->hmax_child[0][0]), sizeof tr->hmax_child);
	
	if(trmap->allocated)	freeTree(letter,inflexion);
	trmap->allocated = false;
	trmap->populated = false;
}

bool QueryResolver::file_treesExists(const char *fname){
	
	FILE *fp;
	const char *mode = "rb";
	bool status;
	
	if((fp = fopen(fname, mode)) == NULL)	status = false;
	else{
		status = true;
		fclose(fp);
	}
	
	return status;
}


void QueryResolver::treeDataInfo(char letter, bool inflexion){
	
	short int numofletter = LETTER(letter)-1;
	switch((int)inflexion){
		case 0:
			tr = &root[numofletter];
			break;
		case 1:
			tr = &inf[numofletter];
			break;
	}
	
	cout<<"Tree Info:"<<endl;
	cout<<"Letter on Tree: "<<tr->letter<<endl;
	cout<<"Tree type: ";
	if(inflexion) cout<<"Inflected"<<endl;
	else cout<<"Root"<<endl;
	cout<<"First letter node exists - ";
	for(short int i=0;i<AZ;i++) cout<<tr->first[i]<<" ";
	cout<<endl;
	cout<<"Counts of First letters on root: "<<tr->letter<<" - ";
	show_array(&(tr->firstnodes[0]), sizeof tr->firstnodes);
	cout<<"ChildTable - "<<endl;
	show_array(&(tr->childtable[0][0]), sizeof tr->childtable, AZ);
	cout<<"RowsChild - "<<endl;
	show_array(&(tr->rows_child[0][0]), sizeof tr->rows_child, AZ);
	cout<<"HMAX_Child - "<<endl;
	show_array(&(tr->hmax_child[0][0]), sizeof tr->hmax_child, AZ);
	
}

short int QueryResolver::freeTree(char letter, bool inflexion){
	
	bool freestatus[26];
	short int restatus;
	short int numletter;
	short int thirdchilds;
	int totbytes;
	int totsizes;
	
	numletter = LETTER(letter) - 1;
	for(short int i=0;i<AZ;i++) freestatus[i] = (bool)1; //has to be true
	restatus = 1;
	
	if(inflexion){
		tr = &inf[numletter];
		trmap = &inf_tree[numletter];
	}
	else{
		tr = &root[numletter];
		trmap = &root_tree[numletter];
	}
	
	for(short int i=0;i<AZ;i++){
		
		if(tr->first[i]){
			thirdchilds = 0;
			totbytes = 0;
			totsizes = 0;
			for(short int j=0;j<AZ;j++){
				if(tr->rows_child[i][j]){
					totbytes = totbytes + tr->rows_child[i][j]*tr->hmax_child[i][j];
					thirdchilds = thirdchilds + 1;
				}
			}
			for(short int k=0;k<thirdchilds;k++){
				if(*(trmap->child[i]+k)){
					totsizes = totsizes + (int)dynarr_size((char *)(*(trmap->child[i]+k)));
					dynarr_free((char *)(*(trmap->child[i]+k)));
				}
			}

			if(totbytes != totsizes) freestatus[i] = (bool)0;
		}
	}
	
	for(short int i=0;i<AZ;i++){
		if(tr->first[i]){
			if(trmap->child[i])	dynarr_free((char **)trmap->child[i]);
		}
	}
	
	if(trmap->bitable) dynarr_free((char *)trmap->bitable);
	
	for(short int i=0;i<AZ;i++){
		if(!freestatus[i]){
			restatus = 0;
			break;
		}
	}
	
	trmap->allocated = (bool)0;
	return restatus;
}

void QueryResolver::showTree(char letter, bool inflexion){
	
	short int numletter;
	short int thirdchilds;

	numletter = LETTER(letter) - 1;

	if(inflexion){
		tr = &inf[numletter];
		trmap = &inf_tree[numletter];
	}
	else{
		tr = &root[numletter];
		trmap = &root_tree[numletter];
	}
	
	if(tr->letter != letter){
		cout<<"Tree does not exist for root: "<<letter<<endl;
	}
	else{
		
		cout<<"Tree for root: "<<letter<<endl;
		for(short int i=0;i<AZ;i++){
			cout<<"First Node: "<<numberedletter(i+1)<<endl;
			if(tr->first[i]){
				thirdchilds = 0;
				for(short int j=0;j<AZ;j++){
					if(tr->rows_child[i][j]){
						thirdchilds = thirdchilds + 1;
					}
				}
				if(thirdchilds){
					short int k=0;
					for(short int j=0;j<AZ;j++){
						if(tr->hmax_child[i][j]){
							if(*(trmap->child[i]+k)){
								show_array(*(trmap->child[i]+k), dynarr_size((char *)*(trmap->child[i]+k)), tr->hmax_child[i][j]);
								k = k + 1;
							}
						}
					}
				}
			}
		cout<<"----------------------------------------------------"<<endl;
		}
		
		cout<<"Bitable: "<<endl;
		show_array(trmap->bitable, dynarr_size((char *)trmap->bitable), 3);
	}
	
	
}

int QueryResolver::populateTree(char letter, bool inflexion, char *bigrid){
	
	bool streamready, stream2ready;
	bool writestatus[2];
	char *node;
	int *ind;
	int *totindices;
	short int *eachlen;
	short int numletter;
	short int lv;
	short int bi;
	int icount;
	short int chexist, chformed;
	string notype;
	string prefix, extension, extension2, ofilename, ofilename2;
	
	if(inflexion){
		notype = "inflexion";
		extension  = INFTREE;
		extension2 = INFDATA;
		
	}
	else{
		notype = "root";
		extension = ROOTTREE;
		extension2 = ROOTDATA;
	}
	
	prefix = letter;
	ofilename = prefix+extension;
	ofilename2 = prefix+extension2;
	
	ofstream outputchilds(ofilename.c_str(), ios::ate | ios::binary);
	if(!outputchilds) streamready = (bool)0;
	else streamready = (bool)1;
	
	ofstream outputdata(ofilename2.c_str(), ios::out | ios::binary);
	if(!outputdata) stream2ready = (bool)0;
	else stream2ready = (bool)1;
	
	writestatus[0] = writestatus[1] = (bool)0;
	chexist = chformed = 0;
	
	numletter = LETTER(letter) - 1;
	
	switch((int)inflexion){
		case 0:
			node = info.node_root;
			ind = info.indices_root;
			totindices = &info.totindices_root;
			eachlen = info.node_root_eachlen;
			tr = &root[numletter];
			trmap = &root_tree[numletter];
			
			break;
			
		case 1:
			node = info.node_inflexion;
			ind = info.indices_inflexion;
			totindices = &info.totindices_inflexion;
			eachlen = info.node_inflexion_eachlen;
			tr = &inf[numletter];
			trmap = &inf_tree[numletter];
			break;
	}

	
	/* may be called by load so setindex with filler to load node required, but a check to load otherwise*/
	for(int i=0;i<AZ;i++){
		if(tr->first[i]){
			setindexinfo(numberedletter(i+1),"second",notype);
			second_last_letterindexer(&info);
			if(*(bigrid+i*3+2) == '!') bi = 0;
			else bi = 1;
			icount = 0;
			lv = 0;
			for(int k=0;k<AZ;k++){
				if(tr->rows_child[i][k]){
					chexist = chexist + 1;
					char xyarray[tr->rows_child[i][k]][tr->hmax_child[i][k]];
					reset_array(&xyarray[0][0], sizeof xyarray);
					char *xy;
					xy = &xyarray[0][0];
					for(int j=0;j<tr->rows_child[i][k];j++){
						memcpy((char *)(xy+j*tr->hmax_child[i][k]), (char *)(node+(*(ind+icount)+bi)*HMAX), sizeof(char)**(eachlen+(*ind+icount)+bi));
						icount = icount + 1;
					}
					
					memcpy((char *)*(trmap->child[i]+lv), (char *)xy, sizeof(char)*tr->hmax_child[i][k]*tr->rows_child[i][k]);
					if(streamready){
						outputchilds.write(&xyarray[0][0], sizeof xyarray);
						chformed = chformed + 1;
					}
					lv = lv + 1;
				}
			}
		}
	}
	
	memcpy((char *)trmap->bitable, (char *)bigrid, sizeof(char)*26*3);
	if(streamready) outputchilds.write(&(*(bigrid+0)), sizeof(char)*26*3);
	
	if(streamready){
		if(inflexion){
			if(chexist == chformed){
				fexist_tree_inf[numletter] = (bool)1;
				writestatus[0] = (bool)1;
			}
			else remove(ofilename.c_str());
		}
		else if(!inflexion){
			if(chexist == chformed){
				fexist_tree_root[numletter] = (bool)1;
				writestatus[0] = (bool)1;
			}
			else remove(ofilename.c_str());
		}
		outputchilds.close();
	}
	
	if(stream2ready){
			outputdata.write((char *)&(*tr), sizeof(struct treedata));
			if(inflexion) fexist_data_inf[numletter] = (bool)1;
			else fexist_data_root[numletter] = (bool)1;
			writestatus[1] = (bool)1;
			outputdata.close();
	}

	if(writestatus[0] == true && writestatus[1] == false){
		remove(ofilename.c_str());
	}
	else if(writestatus[0] == false && writestatus[1] == true){
		remove(ofilename2.c_str());
	}
	
	trmap->populated = (bool)1;
	return 1;
}


int QueryResolver::allocateTree(char letter, bool inflexion){

	short int numletter;
	short int levelone, lv;
	
	numletter = LETTER(letter) - 1;
	
	if(inflexion){
		tr = &inf[numletter];
		trmap = &inf_tree[numletter];
	}
	else{
		tr = &root[numletter];
		trmap = &root_tree[numletter];
	}
	
	
	for(int i=0;i<AZ;i++){
		levelone = 0;
		for(int j=0;j<AZ;j++){
			if(tr->rows_child[i][j]) levelone = levelone + 1;
		}
		if(levelone){
			
			trmap->child[i] = (char **)dynarr_alloc(levelone, sizeof(char *));
			if(!trmap->child[i]) return 0;
			lv = 0;
			for(int k=0;k<AZ;k++){
				
				if(tr->rows_child[i][k]){
					if(tr->hmax_child[i][k] > hmaxofall) hmaxofall = tr->hmax_child[i][k];
					*(trmap->child[i]+lv) = (char *)dynarr_alloc(tr->rows_child[i][k]*tr->hmax_child[i][k],sizeof(char));
					if(!*(trmap->child[i]+lv)) return 0;
					lv = lv + 1;
				}
			}
		}
		
	}
	
	trmap->bitable = (char *)dynarr_alloc(26*3, sizeof(char));
	if(!trmap->bitable) return 0;
	
	trmap->allocated = (bool)1;
	return 1;
}


int QueryResolver::createTree(char letter, bool inflexion){
	//ASSUMPTION - rootwords exist
	
	char bitable[26][3];
	char *node;
	int *ind;
	int *totindices;
	short int *eachlen;
	short int numletter;
	string notype;
	short int bi;
	
	if(inflexion) notype = "inflexion";
	else notype = "root";
	
	numletter = LETTER(letter) - 1;
	
	switch((int)inflexion){
		case 0:
			if(info.letter_onroot != letter) setindexinfo(FILLER,"second",notype,letter); //loading
			node = info.node_root;
			ind = info.indices_root;
			totindices = &info.totindices_root;
			eachlen = info.node_root_eachlen;
			tr = &root[numletter];
			break;
			
		case 1:
			if(info.letter_oninflexion != letter) setindexinfo(FILLER,"second",notype,letter); //loading
			node = info.node_inflexion;
			ind = info.indices_inflexion;
			totindices = &info.totindices_inflexion;
			eachlen = info.node_inflexion_eachlen;
			tr = &inf[numletter];
			break;
	}
	
	initializeTree(letter,inflexion);//initializes
	reset_array(&bitable[0][0], sizeof bitable);

	for(int i=1;i<=AZ;i++){
		setindexinfo(numberedletter(i),"second",notype);
		second_last_letterindexer(&info);
		tr->firstnodes[i-1] = *totindices;
		if(tr->firstnodes[i-1] != 0){
			tr->first[i-1] = (bool)1;
			bitable[i-1][0] = letter;
			bitable[i-1][1] = numberedletter(i);
			if(*(eachlen+*(ind)) != 2) bitable[i-1][2] = '!';
		}
	}

	for(int j=1;j<=AZ;j++){
		setindexinfo(numberedletter(j),"second",notype);
		second_last_letterindexer(&info);//passing is the chunk of second
		for(int i=1;i<=AZ;i++){
			if(bitable[j-1][2] == '!') bi = 0;
			else bi = 1;
			tr->rows_child[j-1][i-1] = treeIndexing(node+*(ind+0)*HMAX, tr->firstnodes[j-1], 2, numberedletter(i), (j-1), letter, inflexion, bi);
			if(tr->rows_child[j-1][i-1]) tr->childtable[j-1][i-1] = numberedletter(i);
		}
	}
	
	if(allocateTree(letter,inflexion) == 0) return 0; //<-- from here little verbose for decisory elses to include
	tr->letter = letter;
	populateTree(letter,inflexion,&bitable[0][0]);
	
	
	
	if(inflexion) status_tinflected[numletter] = (bool)1;
	else status_troot[numletter] = (bool)1;

	return 1;
}


short int QueryResolver::treeIndexing(char *source, int rowcounts, short int inpos, char inletter, short int rowindex, char rootletter, bool inflexion, short int bi){
	//is on the 3rd letter that is 2 index
	//tightly coupled to createTree.
	
	int *ind;
	short int *eachlen;
	int start;
	short int lencounts;
	int row;
	int numberedch;
	int tempnumch;
	short int numletter;
	short int occurences;
	short int *rowstrip;
	short int *hmaxchild;
	
	numletter = LETTER(rootletter) - 1;
	
	switch((int)inflexion){
		case 0:
			ind = info.indices_root;
			eachlen = info.node_root_eachlen;
			tr = &root[numletter];
			rowstrip = &(tr->rows_child[rowindex][0]);
			hmaxchild = &(tr->hmax_child[rowindex][0]);
			break;
			
		case 1:
			ind = info.indices_inflexion;
			eachlen = info.node_inflexion_eachlen;
			tr = &inf[numletter];
			rowstrip = &(tr->rows_child[rowindex][0]);
			hmaxchild = &(tr->hmax_child[rowindex][0]);
			break;
	}

	start = 0;
	lencounts = 0;
	occurences = 0;
	
	numberedch = (int)LETTER(inletter);
	
	for(row=0;row<rowcounts;row++){
		tempnumch = (int)LETTER(*(source+row*HMAX+inpos));
		if(numberedch == tempnumch){
			occurences = occurences + 1;
			lencounts = lencounts + 1;
		}
	}
	
	for(int i=0;i<numberedch-1;i++){
		start = start + *(rowstrip + i);
	}

	if(start!= -1){
		*(hmaxchild+numberedch-1) = maxLen(eachlen+*(ind+start)+bi, lencounts);
	}

	return occurences;
}

short int QueryResolver::maxLen(short int *lensource, int counts){
	
	short int result;
	result = 0;
	
	if(counts == 1) result = *lensource;
	else if(counts > 1){
		
		for(int i=0;i<counts-1;i++){
			if(*(lensource+i) > *(lensource+i+1) && *(lensource+i)>result) result = *(lensource+i);
		}
		if(*(lensource+counts-1) > result) result = *(lensource+counts-1);
	}
	else return 0;
	
	return result;
}


int QueryResolver::loadTree(char letter, bool inflexion){
	
	
	short int numletter;
	string prefix, extension, ofilename;
	
	numletter = LETTER(letter) - 1;
	
	if(inflexion){
		tr = &inf[numletter];
		trmap = &inf_tree[numletter];
		extension = INFDATA;
	}
	else{
		tr = &root[numletter];
		trmap = &root_tree[numletter];
		extension = ROOTDATA;
	}
	
	prefix = letter;
	ofilename = prefix+extension;
	
	/*load tree data*/
	ifstream inputTdata(ofilename.c_str(),ios::in | ios::binary);
	if(!inputTdata)	return 0;
	
	initializeTree(letter,inflexion);
	inputTdata.read((char *)&(*tr), sizeof(struct treedata));
	inputTdata.close();
	
	if(tr->letter == letter){
		if(allocateTree(letter,inflexion) == 1){
			
			if(loadTreePopulation(letter,inflexion) == 1){
				
				trmap->populated = true;
				if(inflexion) status_tinflected[numletter] = true;
				else status_troot[numletter] = true;
			}
			else return 0;
		}
		else return 0;
	}
	else return 0;

	return 1;
}

int QueryResolver::loadTreePopulation(char letter,bool inflexion){
	
	char *biptr;
	char bitable[26][3];
	short int numletter;
	short int lv;
	string prefix, extension, ofilename;
	
	numletter = LETTER(letter) - 1;
	
	if(inflexion){
		tr = &inf[numletter];
		trmap = &inf_tree[numletter];
		extension = INFTREE;
	}
	else{
		tr = &root[numletter];
		trmap = &root_tree[numletter];
		extension = ROOTTREE;
	}
	
	prefix = letter;
	ofilename = prefix+extension;
	
	/*load tree*/
	ifstream inputChilds(ofilename.c_str(),ios::in | ios::binary);
	if(!inputChilds)	return 0;
	
	for(int i=0;i<AZ;i++){
		if(tr->first[i]){
			lv = 0;
			for(int k=0;k<AZ;k++){
				if(tr->rows_child[i][k]){
					
					char xyarray[tr->rows_child[i][k]][tr->hmax_child[i][k]];
					reset_array(&xyarray[0][0], sizeof xyarray);
					char *xy;
					xy = &xyarray[0][0];
					inputChilds.read((char *)xy, sizeof(char)*tr->hmax_child[i][k]*tr->rows_child[i][k]);
					memcpy((char *)*(trmap->child[i]+lv), (char *)xy, sizeof xyarray);
					lv = lv + 1;
				}
			}
		}
	}
	
	reset_array(&bitable[0][0], sizeof bitable);
	biptr = &bitable[0][0];
	inputChilds.read(&(*(biptr+0)), sizeof bitable);
	memcpy((char *)trmap->bitable, (char *)biptr, sizeof bitable);
	
	inputChilds.close();
	return 1;
}


int QueryResolver::loadingforQuery(char letter){
	
	short int status[2];
	short int numletter;
	string prefix;
	string fname;
	string fname2;
	
	status[0] = status[1] = 0;
	numletter = LETTER(letter) - 1;
	prefix = letter;
	
	
	tr = &root[numletter];
	trmap = &root_tree[numletter];
	
	if(updated_root[numletter]){
		if(createTree(letter,0) == 1) status[0] = 1;
	}
	else if(status_troot[numletter] && tr->letter == letter)	status[0] = 1;
	else if(existent_roots[numletter]){
		
		if(fexist_data_root[numletter] && fexist_tree_root[numletter] && !updated_root[numletter]){
				if(loadTree(letter,0) == 1)	status[0] = 1;
		}
		
		if(!fexist_data_root[numletter] && !fexist_tree_root[numletter] && !updated_root[numletter]){
			fname = prefix + ROOTDATA;
			fname2 = prefix + ROOTTREE; 
			fexist_data_root[numletter] = file_treesExists(fname.c_str());
			fexist_tree_root[numletter] = file_treesExists(fname2.c_str());
			
			if(!fexist_data_root[numletter] && !fexist_tree_root[numletter] && !updated_root[numletter]){ //<think here
				if(createTree(letter,0) == 1)	status[0] = 1;
			}
			else{
				if(loadTree(letter,0) == 1) status[0] = 1;
			}
		}
		
	}
	else status[0] = 0;

	
	tr = &inf[numletter];
	trmap = &inf_tree[numletter];
	
	if(updated_inflexion[numletter]){
		if(createTree(letter,1) == 1) status[1] = 1; //on creation other things up the routine happens
	}
	else if(status_tinflected[numletter] && tr->letter == letter) status[1] = 1;
	else if(existent_inflexions[numletter]){
		
		if(fexist_data_inf[numletter] && fexist_tree_inf[numletter] && !updated_inflexion[numletter]){
			if(loadTree(letter,1) == 1) status[1] = 1;
		}
		
		if(!fexist_data_inf[numletter] && !fexist_tree_inf[numletter] && !updated_inflexion[numletter]){
			fname = prefix + INFDATA;
			fname2 = prefix + INFTREE; 
			fexist_data_inf[numletter] = file_treesExists(fname.c_str());
			fexist_tree_inf[numletter] = file_treesExists(fname2.c_str());
			
			if(!fexist_data_inf[numletter] && !fexist_tree_inf[numletter] && !updated_inflexion[numletter]){
				if(createTree(letter,1) == 1) status[1] = 1;
			}
			else{
				if(loadTree(letter,1) == 1) status[1] = 1;
			}
		}
	}
	else status[1] = 0;
	
	if(status[0] || status[1]) return 1;
	else return 0;
}


int QueryResolver::treeQuery(string qsegment){
	
	
	short int hmaxofall;
	int tempmaxofwords;
	short int lowerindices[AZ][2];
	short int seqmarker[AZ];
	short int numletter;
	char *cptr;
	char single, lastch;
	short int treo[5];
	short int count;
	short int ch_rows, ch_hmax, ch_start;
	string currentword;
	short int cmpvalue, lendiff;
	short int cycle;
	
	if(qsegment[0] != INIT && qsegment[0] != TRAN && qsegment[0] != TERM && qsegment[0] != VOC) return -1;
	if(!selected) return -1;
	
	treo[1] = treo[2] = treo[3] = treo[4] = -1; //1 as the root
	cycle = 0;
	numletter = 0;
	hmaxofall = this->hmaxofall;
	tempmaxofwords = maxofwords;
	
	if(master_updated){
		for(short int i=1;i<=AZ;i++){
			if(updated_root[i-1] || updated_inflexion[i-1]){
				loadingforQuery(numberedletter(i));
			}
		}
		master_updated = false;
		if(this->hmaxofall > hmaxofall){
			if(refillSelected(tempmaxofwords, hmaxofall) == 0) return -1; //may be return 0 will not be required but continue with previous alloc
		}
	}
	
	switch(qsegment[0]){
		
		case VOC:
			qsegment.erase(0,1);
			numletter = LETTER(qsegment[0]) - 1;
			
			if(status_troot[numletter]) cycle = 1;
			if(status_tinflected[numletter]) cycle = 2;
			
			if(qsegment.length() == 1 && cycle != 0) return 1; //for time being 
			else if(qsegment.length() == 2){
				
				treo[2] = LETTER(qsegment[1]) - 1;
				for(short int a=1;a<=cycle;a++){
					if(a == 1){
						trmap = &root_tree[numletter];
						tr = &root[numletter];
					}
					else{
						trmap = &inf_tree[numletter];
						tr = &inf[numletter];
					}
					cptr = trmap->bitable;
					if(tr->first[treo[2]]){
						
						if(*(cptr+treo[2]*3+1) != FILLER) if(*(cptr+treo[2]*3+2) == FILLER) return 1;
					}
				}
				return 0;
			}
			else if(qsegment.length() >= 3){
				
				for(short int a=1;a<=cycle;a++){
					
					if(a == 1){
						
						trmap = &root_tree[numletter];
						tr = &root[numletter];
					}
					else{
						trmap = &inf_tree[numletter];
						tr = &inf[numletter];
					}

					treo[2] = LETTER(qsegment[1]) - 1;
					treo[3] = LETTER(qsegment[2]) - 1;

					if(qsegment.length() > 3) treo[4] = LETTER(qsegment[3]) - 1;
					count = -1;
					if(tr->first[treo[2]]){
						
						reset_array(&seqmarker[0], sizeof seqmarker, 26, -1);
						for(short int i=0;i<AZ;i++){
							if(tr->rows_child[treo[2]][i]){
								count = count + 1;
								seqmarker[i] = count;
							}
						}
						cptr = *(trmap->child[treo[2]]+seqmarker[treo[3]]);
						ch_rows = tr->rows_child[treo[2]][treo[3]];
						ch_hmax = tr->hmax_child[treo[2]][treo[3]];
						if(qsegment.length() == 3){
							currentword = makestring(cptr,0,ch_hmax);
							cmpvalue = (short int)findPattern(qsegment, currentword);
							if(cmpvalue != -1){
								if(currentword.length() == qsegment.length()){
									reset_array(&matchword[0][0], sizeof matchword);
									memcpy(&matchword[0][0], (char *)(cptr+0*ch_hmax+0), sizeof(char)*ch_hmax);
									return 1;
								}
							}
						}
						else if(qsegment.length() > 3){
							reset_array(&lowerindices[0][0], sizeof lowerindices, AZ*2, -1);
							fourth_indexer(cptr, &lowerindices[0][0], ch_rows, ch_hmax);
							ch_start = (short int)fourth_indexer(cptr, &lowerindices[0][0], ch_rows, ch_hmax, treo[4]);
							for(short int i=lowerindices[ch_start][1] + 1; i<=lowerindices[treo[4]][1]; i++){
								currentword = makestring(cptr, i, ch_hmax);
								cmpvalue = (short int)findPattern(qsegment, currentword);
								if(cmpvalue != -1){
									if(currentword.length() == qsegment.length()){
										reset_array(&matchword[0][0], sizeof matchword);
										memcpy(&matchword[0][0], (char *)(cptr+i*ch_hmax+0), sizeof(char)*ch_hmax);
										return 1;
									}
								}
							}
						}
					}
					
				}
				return 0;
			}
			
			break;
		
		case INIT:
			qsegment.erase(0,1);
			numletter = LETTER(qsegment[0]) - 1;
			
			if(status_troot[numletter]) cycle = 1;
			if(status_tinflected[numletter]) cycle = 2;
			
			for(short int a=1;a<=cycle;a++){
				if(a == 1){
					trmap = &root_tree[numletter];
					tr = &root[numletter];
				}
				else{
					trmap = &inf_tree[numletter];
					tr = &inf[numletter];
				}
				
				treo[2] = LETTER(qsegment[1]) - 1;
				
				if(qsegment.length() >= 2){
					count = -1;
					if(tr->first[treo[2]]){
						reset_array(&seqmarker[0], sizeof seqmarker, 26, -1);
						for(short int j=0;j<AZ;j++){
							if(tr->rows_child[treo[2]][j]){
								count = count + 1;
								seqmarker[j] = count;
							}
						}
						if(qsegment.length() == 2){
							for(short int j=0;j<AZ;j++){
								if(seqmarker[j] != -1){
									cptr = *(trmap->child[treo[2]]+seqmarker[j]);
									ch_rows = tr->rows_child[treo[2]][j];
									ch_hmax = tr->hmax_child[treo[2]][j];
									for(short int r=0;r<ch_rows;r++){
										memcpy((char *)(selected+listmarker*hmaxofall+0), (char *)(cptr+r*ch_hmax+0), sizeof(char)*ch_hmax);
										listmarker = listmarker + 1;
									}
								}
							}
						}
						else if(qsegment.length() > 2){
							treo[3] = LETTER(qsegment[2]) - 1;
							if(qsegment.length() > 3) treo[4] = LETTER(qsegment[3]) - 1;
							cptr = *(trmap->child[treo[2]]+seqmarker[treo[3]]);
							ch_rows = tr->rows_child[treo[2]][treo[3]];
							ch_hmax = tr->hmax_child[treo[2]][treo[3]];
							if(treo[4] != -1){
								reset_array(&lowerindices[0][0], sizeof lowerindices, AZ*2, -1);
								fourth_indexer(cptr, &lowerindices[0][0], ch_rows, ch_hmax);
								ch_start = (short int)fourth_indexer(cptr, &lowerindices[0][0], ch_rows, ch_hmax, treo[4]);
								for(short int r=lowerindices[ch_start][1] + 1; r<=lowerindices[treo[4]][1]; r++){
									cout<<"In tree query init"<<endl;
									currentword = makestring(cptr, r, ch_hmax);
									cmpvalue = (short int)findPattern(qsegment, currentword);
									if(cmpvalue == 0){
											memcpy((char *)(selected+listmarker*hmaxofall+0), (char *)(cptr+r*ch_hmax+0), sizeof(char)*ch_hmax);
											listmarker = listmarker + 1;
									}
								}
							}
							else{
								
								for(short int r=0; r<ch_rows; r++){
									memcpy((char *)(selected+listmarker*hmaxofall+0), (char *)(cptr+r*ch_hmax+0), sizeof(char)*ch_hmax);
									listmarker = listmarker + 1;
								}
							}
							
						}
					}
				}
			}
			
			break;

		case TERM:
			qsegment.erase(0,1);

			if(qsegment.length() >= 2){
				
				for(short int i=1;i<=AZ;i++){
					
					if(status_troot[i-1]) cycle = 1;
					if(status_tinflected[i-1]) cycle = 2;
					
					for(short int a=1;a<=cycle;a++){
						
						if(a == 1){
							trmap = &root_tree[i-1];
							tr = &root[i-1];
						}
						else{
							trmap = &inf_tree[i-1];
							tr = &inf[i-1];
						}
						
						lastch = qsegment[qsegment.length()-1];
					
						for(short int i=0;i<AZ;i++){
							count = -1;
							if(tr->first[i]){
								reset_array(&seqmarker[0], sizeof seqmarker, 26, -1);
								for(short int j=0;j<AZ;j++){
									if(tr->rows_child[i][j]){
										count = count + 1;
										seqmarker[j] = count;
									}
								}
								for(short int j=0;j<AZ;j++){
									if(seqmarker[j] != -1){
										cptr = *(trmap->child[i]+seqmarker[j]);
										ch_rows = tr->rows_child[i][j];
										ch_hmax = tr->hmax_child[i][j];
										for(short int r=0;r<ch_rows;r++){
											currentword = makestring(cptr, r, ch_hmax);
											if(currentword[currentword.length()-1] == lastch){
												cmpvalue = findPattern(qsegment,currentword);
												lendiff = currentword.length()-qsegment.length();
												if(cmpvalue == lendiff && cmpvalue > -1){
													memcpy((char *)(selected+listmarker*hmaxofall+0), (char *)(cptr+r*ch_hmax+0),sizeof(char)*ch_hmax);
													listmarker = listmarker + 1;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			
			break;
			
		case TRAN:
			qsegment.erase(0,1);
			
			if(qsegment.length() >= 2){
				
				for(short int i=1;i<=AZ;i++){
					
					if(status_troot[i-1]) cycle = 1;
					if(status_tinflected[i-1]) cycle = 2;
					
					for(short int a=1;a<=cycle;a++){
						
						if(a == 1){
							trmap = &root_tree[i-1];
							tr = &root[i-1];
						}
						else{
							trmap = &inf_tree[i-1];
							tr = &inf[i-1];
						}
						
						for(short int i=0;i<AZ;i++){
							count = -1;
							if(tr->first[i]){
								reset_array(&seqmarker[0], sizeof seqmarker, 26, -1);
								for(short int j=0;j<AZ;j++){
									if(tr->rows_child[i][j]){
										count = count + 1;
										seqmarker[j] = count;
									}
								}
								for(short int j=0;j<AZ;j++){
									if(seqmarker[j] != -1){
										cptr = *(trmap->child[i]+seqmarker[j]);
										ch_rows = tr->rows_child[i][j];
										ch_hmax = tr->hmax_child[i][j];
										for(short int r=0;r<ch_rows;r++){
											currentword = makestring(cptr, r, ch_hmax);
											cmpvalue = findPattern(qsegment,currentword);
											if(cmpvalue > -1){
												memcpy((char *)(selected+listmarker*hmaxofall+0), (char *)(cptr+r*ch_hmax+0),sizeof(char)*ch_hmax);
												listmarker = listmarker + 1;
											}
										}
									}
								}
							}
						}
					}
				}
			}

			break;

	}

	if(listmarker > 0)	show_array(selected, dynarr_size((char *)selected), hmaxofall, listmarker*hmaxofall);
	return 0;
}


int QueryResolver::fourth_indexer(char *storage, signed short int *indexer_ptr, short int counts, short int hlen, short int fourth_ind){
	
	short int start;
	short int numch;
	short int singleton, i;
	singleton = 3;
	
	if(fourth_ind == -1){
		for(short int row=0;row<counts;row++){
			numch=LETTER(*(storage+row*hlen+singleton)) - 1;
			*(indexer_ptr+numch*2+0)=numch;
			*(indexer_ptr+numch*2+1)=row;
		}
		return 1;
	}
	else{
		singleton = 1;
		i = 1;
		
		while(*(indexer_ptr + (fourth_ind-singleton)*2+ 1) + 1 == 0 && i<fourth_ind){
				singleton = singleton + 1;
				i = i + 1;
		}
		
		return fourth_ind-singleton;
	}
		
}


int QueryResolver::refillSelected(int tmaxofwords, short int thmaxofall){
	
	char buffselected[SELECTEDMAX][hmaxofall];
	reset_array(&buffselected[0][0], sizeof buffselected);
	char *buffptr;
	buffptr = &buffselected[0][0];
	for(int i=0;i<listmarker;i++){
		memcpy((char *)(buffptr+ i*hmaxofall + 0), (char *)(selected + i*thmaxofall + 0), sizeof(char)*thmaxofall);
	}
	if(selected)  dynarr_free((char *)selected);
	selected = (char *)dynarr_alloc(SELECTEDMAX*hmaxofall, sizeof(char));
	if(selected){
		reset_array(selected, dynarr_size((char *)selected));
		memcpy((char *)selected, (char *)buffptr, sizeof(char)*SELECTEDMAX*hmaxofall);
		return 1;
	}
	else return 0;
	
}


void QueryResolver::showmword(){
	show_array(&matchword[0][0], sizeof matchword);
}

void QueryResolver::structinfo(){
	
	cout<<endl<<"Structinfo:"<<endl;
	
	cout<<"Letter on Root: "<<info.letter_onroot<<endl;
	if(info.letter_onroot != FILLER){
		if(*(info.node_root+0) == info.letter_onroot) cout<<"Node Root loaded with: "<<info.letter_onroot<<endl;
	}
	else if(info.node_root) cout<<"Node Root opened"<<endl;
	else cout<<"Node Root closed"<<endl;
	cout<<"Node Root wordcounts: "<<info.node_root_wordcounts<<endl;
	if(info.node_root_eachlen) cout<<"Node Root eachlen opened"<<endl;
	else cout<<"Node Root eachlen closed"<<endl;
	if(info.indices_root) cout<<"Node Root indices opened"<<endl;
	else cout<<"Node Root indices closed"<<endl;
	cout<<"Root indices counts: "<<info.totindices_root<<endl;
	
	cout<<endl;
	cout<<"Letter on Inflexion: "<<info.letter_oninflexion<<endl;
	if(info.letter_oninflexion != FILLER){
		if(*(info.node_inflexion+0) == info.letter_oninflexion) cout<<"Node Inflexion loaded with: "<<info.letter_oninflexion<<endl;
	}
	else if(info.node_inflexion) cout<<"Node Inflexion opened"<<endl;
	else cout<<"Node Inflexion closed"<<endl;
	cout<<"Node Inflexion wordcounts: "<<info.node_inflexion_wordcounts<<endl;
	if(info.node_inflexion_eachlen) cout<<"Node Inflexion eachlen opened"<<endl;
	else cout<<"Node Inflexion eachlen closed"<<endl;
	if(info.indices_inflexion) cout<<"Node Inflexion indices opened"<<endl;
	else cout<<"Node Inflexion indices closed"<<endl;
	cout<<"Inflexion indices counts: "<<info.totindices_inflexion<<endl;
	
	cout<<endl;
	cout<<"Letter to index for: "<<info.index_for<<endl;
	cout<<"To index position: "<<info.pos;
	if(info.pos == userdata::second) cout<<" Second"<<endl;
	else if(info.pos == userdata::last) cout<<" Last"<<endl;
	cout<<"For nodetype: "<<info.ntype;
	if(info.ntype == userdata::root) cout<<" Root"<<endl;
	else if(info.ntype == userdata::inflexion) cout<<" Inflexion"<<endl;
}

void QueryResolver::showuserdata(){
	
	cout<<"Filesizes: Roots"<<endl;
	for(int i=0;i<AZ;i++) if(root_filesize[i][0]) cout<<numberedletter(i+1)<<REXT<<":"<<root_filesize[i][1]<<endl;
	cout<<"Filesizes: Inflexion"<<endl;
	for(int i=0;i<AZ;i++) if(inflexion_filesize[i][0]) cout<<numberedletter(i+1)<<IEXT<<":"<<inflexion_filesize[i][1]<<endl;
}

void QueryResolver::showmaxtypes(){
	cout<<"Max int: "<<std::numeric_limits<int>::max()<<endl;
	cout<<"Max unsigned int: "<<std::numeric_limits<unsigned int>::max()<<endl;
	cout<<"Max short int: "<<std::numeric_limits<short int>::max()<<endl;
	cout<<"Max unsigned short int: "<<std::numeric_limits<unsigned short int>::max()<<endl;
}

QueryResolver::QueryResolver(){
	
	cout<<"Constructing QueryResolver"<<endl;
	maxofwords = 0;
	hmaxofall = 0;
	listmarker=0;
	initiateinfo();
	reset_array(&root_filesize[0][0], sizeof root_filesize);
	reset_array(&inflexion_filesize[0][0], sizeof inflexion_filesize);
	structinfo();
	lastseek_root = FILLER;
	lastseek_inflexion = FILLER;
	reset_array(&matchword[0][0], sizeof matchword);
	
	for(short int i=0;i<AZ;i++){
		status_troot[i] = (bool)0;
		status_tinflected[i] = (bool)0;
		root[i].letter = FILLER;
		inf[i].letter = FILLER;
		root_tree[i].allocated = (bool)0;
		root_tree[i].populated = (bool)0;
		inf_tree[i].allocated = (bool)0;
		inf_tree[i].populated = (bool)0;
		fexist_data_inf[i] = false;
		fexist_data_root[i] = false;
		fexist_tree_inf[i] = false;
		fexist_tree_root[i] = false;
	}
	
	for(short int i=1;i<=AZ;i++){
		loadingforQuery(numberedletter(i));
		master_updated = false;
	}
	
	for(short int i=0;i<AZ;i++){
		if(words_each_root[i][1] > maxofwords) maxofwords = words_each_root[i][1];
		if(wordsinflected_each_root[i][1] > maxofwords) maxofwords = wordsinflected_each_root[i][1];
	}
	
	selected = (char *)dynarr_alloc(SELECTEDMAX*hmaxofall, sizeof(char));
	cout<<"Selected size: "<<dynarr_size((char *)selected)<<endl;
	cout<<"Maxinwords: "<<maxofwords<<endl;
	cout<<"Hmaxofall: "<<hmaxofall<<endl;
	reset_array(selected, dynarr_size((char *)selected));
	
}

void QueryResolver::free_userdata(){
	
	if(info.node_root) dynarr_free((char *)info.node_root);
	if(info.node_root_eachlen) dynarr_free((short int *)info.node_root_eachlen);
	if(info.indices_root)	dynarr_free((int *)info.indices_root);
	
	if(info.node_inflexion)	dynarr_free((char *)info.node_inflexion);
	if(info.node_inflexion_eachlen) dynarr_free((short int *)info.node_inflexion_eachlen);
	if(info.indices_inflexion)	dynarr_free((int *)info.indices_inflexion);
	cout<<"Userdata freed"<<endl;
}

QueryResolver::~QueryResolver(){
	
	char ch;
	
	cout<<"Destructing QueryResolver"<<endl;
	free_userdata();
	if(selected){
		dynarr_free((char *)selected);
		cout<<"Selected buffer freed"<<endl;
	}
	
	for(short int i=0;i<AZ;i++){
		ch = numberedletter(i+1);
		if(status_troot[i]){
			cout<<"Tree: type-root: "<<ch<<" ,";
			cout<<" Tree freed: "<<freeTree(ch,0)<<endl;
		}
		if(status_tinflected[i]){
			cout<<"Tree: type-inflected: "<<ch<<" ,";
			cout<<"Tree freed: "<<freeTree(ch,1)<<endl;
		}
	}
	
}


void ADD_USER(){

	cout<<"Users are "<<users<<"\n";
	switch(users){
		case 0:
			try{
					::buffptr = new Words [users+1];
				}catch(bad_alloc xa){
					cout<<"Failure\n";
				}
				users=users+1;
				Gptr[users-1]=&(Words&)(*(::buffptr+users-1));
				//ptr=&(Create_CharMemory&)(*(buffptr+users-1));
				break;
		default:
			Words *tempobject;
			try{
					tempobject = new Words [users];
				}catch(bad_alloc xa){
					cout<<"Failure\n";
				}
			for(int i=0;i<users;i++){
					tempobject[i].create_object((Words&)(*Gptr[i]));
				}
			
			delete [] ::buffptr;
			try{
					::buffptr = new Words [users+1];
				}catch(bad_alloc xa){
					cout<<"Failure\n";
				} 
				users=users+1;
				for(int i=0;i<users-1;i++){
						::buffptr[i].create_object(tempobject[i]);
					}
				for(int i=0;i<users;i++) Gptr[i]=&(Words&)(*(::buffptr+i));
				delete [] tempobject;
		}
}

void FREE_OBJECTS(){
	delete [] ::buffptr;
}


int main(){
	
	int result;
	string word1="nefarious";
	string word2="factotum";
	string word3="posterior";
	string word4="facade";
	string word5="fastidous";
	string word6="fable";
	string word7="mono";


	//working method to be used again START
	
	Words object;
	object.AssignUser_to_object("Myself");
	object.push_user_datums(".factotum");
	object.push_user_datums(".absolute");
	object.push_user_datums(".deride");
	object.push_user_datums(".nefarious");
	object.push_user_datums(".tinsel");
	object.push_user_datums(".fable");
	object.push_user_datums("~tion");
	object.push_user_datums(".mono");
	object.push_user_datums("~arted");
	object.push_user_datums("_che");
	object.push_user_datums(".zebra");
	object.push_user_datums(".tin");
	object.push_user_datums("~byl");
	object.push_user_datums("~ment");
	object.push_user_datums("_gar");
	object.push_user_datums("_der");
	object.push_user_datums("_art");
	object.push_user_datums(".fa");
	object.push_user_datums(".f");
	object.push_user_datums("~arted");
	object.push_user_datums("~a");
	object.push_user_datums("~byl");	
	object.show_usernodeinfo();
	
	//result=object.passdata(std::cin,std::cout); //<-- use for writing words
	
	//END

	//BELOW 3 LINES WERE COMMENTED at 2022 when i opened it
	Qobject[0].AssignUser_to_object("Nathan");
	Qobject[0].show_usernodeinfo();
	cout<<endl;

	//added 2 dec 2022
	cout<<"Showing tree";
	Qobject[0].showTree('t',false);
	
	
	

	short int output;
	string seg;	
	while(1){
		
		cout<<"Query:";
		cin>>seg;
		if(seg == "@") break;
		output = Qobject[0].treeQuery(seg);
		
		if(output && seg[0]=='.'){
			if(seg.length() == 3) cout<<"Outcome: "<<output<<endl;
			else if(seg.length() > 3) Qobject[0].showmword();
		}
		else if(!output && seg[0]=='.') cout<<"Outcome: "<<output<<endl;

	}





	//IMPORTANT
	/*
	cout<<"\nmy alphabet numeric\n";
	cout<<object.LETTER('m');
	
	for(short int i=0;i<26;i++){
		for(short int j=0;j<2;j++){
			dex[i][j]=-1;
		}
	}
	object.array_indexer(s,dx,10,1);
	cout<<"\n\n";
	for(short int i=0;i<26;i++){
		cout<<dex[i][0]<<" ";
		cout<<dex[i][1];
		cout<<"\n";
		}
	*/
	
	
	//DYNAMIC OBJECT CALLING
	//TO BE USED object count set to 1
	//START
	/*
	string username;
	int objcount;
	objcount=0;
	while(objcount<1){		
		cout<<"Enter user : ";
		cin>>username;
		ADD_USER();
		Gptr[objcount]->AssignUser_to_object(username.c_str());
		Gptr[objcount]->push_user_datums(".factotum");
		Gptr[objcount]->push_user_datums(".absolute");
		Gptr[objcount]->push_user_datums(".deride");
		Gptr[objcount]->push_user_datums(".nefarious");
		Gptr[objcount]->push_user_datums(".tinsel");
		Gptr[objcount]->push_user_datums(".fable");
		Gptr[objcount]->push_user_datums("~tion");
		Gptr[objcount]->push_user_datums(".mono");
		Gptr[objcount]->push_user_datums("~arted");
		Gptr[objcount]->push_user_datums("_che");
		Gptr[objcount]->push_user_datums(".zebra");
		Gptr[objcount]->push_user_datums(".tin");
		Gptr[objcount]->push_user_datums("~byl");
		Gptr[objcount]->push_user_datums("~ment");
		Gptr[objcount]->push_user_datums("_gar");
		Gptr[objcount]->push_user_datums("_der");
		Gptr[objcount]->push_user_datums("_art");
		Gptr[objcount]->push_user_datums(".fa");
		Gptr[objcount]->push_user_datums(".f");
		Gptr[objcount]->push_user_datums("~arted");
		Gptr[objcount]->push_user_datums("~a");
		Gptr[objcount]->push_user_datums("~byl");
		objcount++;
	}
	cout<<"=====\n";
	for(int i=0;i<1;i++){
		 Gptr[i]->show_usernodeinfo();
		 cout<<"\n\n";
	 }
	 
	result=Gptr[0]->passdata(std::cin,std::cout);
	*/
	//END


	//FREE_OBJECTS();
	

	return 0;
}


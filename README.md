# Rhyming-Words
**Search for word and get similar sounding words for it**

>**About Project:** This project was the start of my journey with programming. And I chose C++ as the language behind it. The basic and the foremost idea was to search a word and get similar sounding words as a result. But as I started, it grew tremendously as I had to look at the other components of creating the desired outcome. 
 - For now, the program does produce results like searching a word **containing** any subset of letters within the word.
 - Produce search outcome of words **ending** with any subset of letters
 - Produce search outcome of **exact** word search
 - Produce search outcome of words **starting** with any subset of letters
 - It has a full functionality of writing words and inflected forms in text files.
 - Load all the words of a particular user from the files into heap.
 - **This heap gets in form of nodes (26 nodes from letter a-z). Each node in the heap will have 26 subnodes (meaning aa, ab, ac, ad, ae, af ..... az = 26      child nodes). Similarly there will be 26 childnodes for next letter b (ba, bb, bc, bd ... bz)**
 - This tree in the heap is sorted lexically for all the words inside each child node.
 - As data is already loaded in the heap once the program starts, search functions or queryresolver looks up in the tree by traversing through it and          arriving at the word given in query in less iterations.
 
 
 
 ## Significant feature of an aggressive fast search: As you read above, it is a lexical tree of all data in the heap.
 
 **Basic Query types:**
 
 **_tra = (would mean search words starting with "tra") - defined as INIT(in the code) - (initiating)**
 
 **+er = (would mean search for words containing "er" in between) - defined as TRAN(in the code) - (transient)**
 
 **~et = (would mean search for words ending with "et") - defined as TERM(in the code) - (terminating)**
 
 **.walk = (would mean search the exact word "walk" - defined as VOC(in the code) - (vocubulary exact)**
 
 **Now See the below output which shows data for letter - "T"(root) and childnode data with words starting with "tr"**
 
 ![image](https://user-images.githubusercontent.com/26901597/205448611-4790ff7f-a70e-4a9d-b797-100ba90b2b8c.png)

**Now I am making a query = _trap . By looking at the data you will expect that it will take 5 iterations to reach word - "trap" as it would need 5 comparisions. But thats not true. Look at the line in below screenshot which is a print statement - "In tree query init" which basically signifies iterations. It will only take 1 iteration to reach to that row**

![image](https://user-images.githubusercontent.com/26901597/205448368-6297213d-ee77-4c86-91bf-dff768b405f4.png)

## Lexical Tree in the heap


![image](https://user-images.githubusercontent.com/26901597/205473020-81461029-a794-4c36-b513-5f3a2d521770.png)

 
 
 
  The main idea of producing rhyming words is not yet developed. This code is around **3068** lines. Heavy usage of pointers throughout the program. This     project is made from ground up and even the **lexical sorter** was developed by me. There are 2 classes -  Words, Queryresolver
  
  The project duration was 7 months in 2018 and after that I left it unfinished and the only work left is to put the backbone mechanism - to search rhyming   words for a word provided.
  
  >**Learnings:** Heavy usage of pointers like struct pointers, struct pointer to pointer variable inside struct, complex forms of pointers, pointer accessing 2 dimensional grids, heap allocation, double pointers.
  
  Looking forward to create the backbone of this project when I get time.....




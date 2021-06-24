#include<iostream>
#include"BPlusTree.h"

int main()
{
	BPlusTree<int> *tree = new BPlusTree<int>("my.index", 4, 455);
	tree->Insert(2, 44);
	tree->Insert(3, 56);
	tree->Insert(4, 78);
	tree->Insert(7, 44);
	tree->Insert(9, 44);
	tree->Insert(6, 44);
	tree->printleaf();
	
	
	tree->Delete(2);
	tree->printleaf();
	
	cout<< tree->Search(4)<<endl;
	
	tree->WriteBack(); 
	
	
	BPlusTree<int> *tree1 = new BPlusTree<int>("my.index", 4, 455);
	tree1->ReadTree();
	tree1->printleaf();
	
	cout<<tree1->Search(3)<<endl;
 } 

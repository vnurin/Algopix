//
//  HufImage.cpp
//  algopix
//
//  Created by Vahagn Nurijanyan on 2018-09-14.
//  Copyright © 2018 BABELONi INC. All rights reserved.
//

#include "HufImage.hpp"
#include "io.hpp"

uint32_t HufImage::TripleSize(uint32_t* count,std::vector<uint32_t> lengths[])
{
    uint32_t huf_q[LHQ];
    uint32_t huf_size=13;
    int q_h;
    int z_max;
    int z_step;
    if(lengths[1].size()!=0 && lengths[2].size()!=0) {
        Presented=0;
        z_step=1;
        z_max=3;
    }
    else if(lengths[2].size()==0) {
        Presented=1;
        z_step=1;
        z_max=2;
        QLHL[2]=0;
    }
    else// if(lengths[1].size()==0)
    {
        Presented=2;
        z_step=2;
        z_max=3;
        QLHL[1]=0;
    }
    for(int z=0;z<z_max;z+=z_step)
    {
        uint32_t len;
        std::vector<uint32_t>* length=&lengths[z];
        for(int i=0;i<LHQ;i++)
            huf_q[i]=0;
        long ql=length->size();
        for(long i=0;i<ql;i++)
        {
            len=(*length)[i];
            while(len>=4097)
            {
                len-=4097;
                huf_q[LHQ-1]++;
            }
            for(int j=0;j<LHQ;j++)
                if(len<Borders1[j])
                {
                    huf_q[j]++;
                    break;
                }
        }
        q_h=0;
        for(int i=LHQ-1;i>=0;i--)
            if(huf_q[i]!=0)
            {
                q_h=i+1;
                break;
            }
        if(q_h>2)
        {
            for(int i=4;i<q_h;i++)
                huf_size+=huf_q[i]*ExtraBits2[i];
            std::vector<SORT_HUFFMAN> huf_sort;
            SORT_HUFFMAN temp;
            for(int i=0;i<q_h;i++)
            {
                temp.count=huf_q[i];
                temp.index=i;
                huf_sort.push_back(temp);
            }
            sort(huf_sort.begin(),huf_sort.end());
            int q_z;
            for(int i=0;i<q_h;i++)
                if(huf_sort[i].count!=0)
                {
                    q_z=i;
                    break;
                }
            for(int i=q_z;i<q_h;i++)
                huf_q[i-q_z]=huf_sort[i].count;
            int q_nz=q_h-q_z;
            uint32_t huf_b[LHQ];
            for(int i=0;i<q_nz;i++)
                huf_b[i]=huf_q[i];
            if(q_nz==1)
                huf_b[0]=1;
            else
                Huffman(huf_b,q_nz);
            for(int i=0;i<q_z;i++)
                LHufLCodeLength[z][huf_sort[i].index]=0;
            for(int i=q_z;i<q_h;i++)
                LHufLCodeLength[z][huf_sort[i].index]=huf_b[i-q_z];
//            huf_sort.clear();
            huf_size+=HuffmanTableSize(LHufLCodeLength[z],q_h);
            for(int i=0;i<q_nz;i++)
                huf_size+=huf_b[i]*huf_q[i];
        }
        else if(q_h==2)
        {
            if(huf_q[0]==0)
                LHufLCodeLength[z][0]=0;
            else
                LHufLCodeLength[z][0]=1;
            huf_size+=1+ql;
        }
        if(Presented==0)
            huf_size+=ql;
        QLHL[z]=q_h;
    }
//    if(QLHL[1]>0) {
        for (int i=0; i<lengths[1].size(); i++) {
//FSTR.            huf_size+=lengths[1][i]+1;
            huf_size+=lengths[1][i];
        }
        huf_size+=lengths[1].size();//FSTR.
//    }
//    if(QLHL[2]>0) {
    if(Presented!=1) {
        huf_size+=4;
        q_h=0;
        for(int i=SHQ-1;i>=0;i--)
            if(count[i]!=0)
            {
                q_h=i+1;
                break;
            }
        if(q_h>2)
        {
            std::vector<SORT_HUFFMAN> huf_sort;
            SORT_HUFFMAN temp;
            for(int i=0;i<q_h;i++)
            {
                temp.count=count[i];
                temp.index=i;
                huf_sort.push_back(temp);
            }
            sort(huf_sort.begin(),huf_sort.end());
            int q_z;
            for(int i=0;i<q_h;i++)
                if(huf_sort[i].count!=0)
                {
                    q_z=i;
                    break;
                }
            for(int i=q_z;i<q_h;i++)
                count[i-q_z]=huf_sort[i].count;
            int q_nz=q_h-q_z;
            uint32_t huf_b[SHQ];
            for(int i=0;i<q_nz;i++)
                huf_b[i]=count[i];
            if(q_nz==1)
                huf_b[0]=1;
            else
                Huffman(huf_b,q_nz);
            for(int i=0;i<q_z;i++)
                SHufCodeLength[huf_sort[i].index]=0;
            for(int i=q_z;i<q_h;i++)
                SHufCodeLength[huf_sort[i].index]=huf_b[i-q_z];
//            huf_sort.clear();
            huf_size+=HuffmanTableSize(SHufCodeLength,q_h);
            for(int i=0;i<q_nz;i++)
                huf_size+=(huf_b[i]+1)*count[i];
        }
        else if(q_h==2)
        {
            if(count[0]==0)
                SHufCodeLength[0]=0;
            else
                SHufCodeLength[0]=1;
            huf_size+=1+2*(count[0]+count[1]);
        }
        else if(q_h==1)
        {
            huf_size+=count[0];
        }
        QSH=q_h;
    }
    return huf_size;
}

uint32_t HufImage::DeltaSize(uint32_t* count,uint32_t* count1,uint32_t* count3)
{
    uint32_t huf_size=4;
    uint32_t huf_q[HQ];
    int a,b=1;
    for(int i=0;i<HQ;i++)
    {
        a=b;
        b+=Group[i];
        huf_q[i]=0;
        for(int j=a;j<b;j++)
            huf_q[i]+=count[j];
    }
    int q_h=0;
    for(int i=HQ-1;i>=0;i--)
        if(huf_q[i]!=0)
        {
            q_h=i+1;
            break;
        }
    if(q_h>2)
    {
        for(int i=EB;i<q_h;i++)
            huf_size+=huf_q[i]*ExtraBits[i];
        std::vector<SORT_HUFFMAN> huf_sort;
        SORT_HUFFMAN temp;
        for(int i=0;i<q_h;i++)
        {
            temp.count=huf_q[i];
            temp.index=i;
            huf_sort.push_back(temp);
        }
        sort(huf_sort.begin(),huf_sort.end());
        int q_z;
        for(int i=0;i<q_h;i++)
            if(huf_sort[i].count!=0)
            {
                q_z=i;
                break;
            }
        for(int i=q_z;i<q_h;i++)
            huf_q[i-q_z]=huf_sort[i].count;
        int q_nz=q_h-q_z;
        uint32_t huf_b[HQ];
        for(int i=0;i<q_nz;i++)
            huf_b[i]=huf_q[i];
        if(q_nz==1)
            huf_b[0]=1;
        else
            Huffman(huf_b,q_nz);
        for(int i=0;i<q_z;i++)
            HufCodeLength[huf_sort[i].index]=0;
        for(int i=q_z;i<q_h;i++)
            HufCodeLength[huf_sort[i].index]=huf_b[i-q_z];
        huf_size+=HuffmanTableSize(HufCodeLength,q_h);
        for(int i=0;i<q_nz;i++)
            huf_size+=huf_b[i]*huf_q[i];
    }
    else if(q_h==2)//if(q_h!=1)
    {
        if(count[1]==0)
            HufCodeLength[0]=0;
        else
            HufCodeLength[0]=1;
        huf_size+=1+count[1]+count[2];
    }
    QH=q_h;
    if(QLHL[1]>0) {
        huf_size+=3;
        for(q_h=6;q_h>=0;q_h--)
            if(count3[q_h]!=0)
            {
//QH3.                q_h++;
                break;//this always happens because QH3!=0 for sure
            }
//QH3.        QH3=q_h;
        QH3=++q_h;//QH3.
        if(QH3>1) {
            if(q_h>2)
            {
                std::vector<SORT_HUFFMAN> huf_sort;
                SORT_HUFFMAN temp;
                for(int i=0;i<q_h;i++)
                {
                    temp.count=count3[i];
                    temp.index=i;
                    huf_sort.push_back(temp);
                }
                sort(huf_sort.begin(),huf_sort.end());
                int q_z;
                for(int i=0;i<q_h;i++)
                    if(huf_sort[i].count!=0)
                    {
                        q_z=i;
                        break;
                    }
                for(int i=q_z;i<q_h;i++)
                    count3[i-q_z]=huf_sort[i].count;
                int q_nz=q_h-q_z;
                uint32_t huf_b[HQ3];
                for(int i=0;i<q_nz;i++)
                    huf_b[i]=count3[i];
                if(q_nz==1)
                    huf_b[0]=1;
                else
                    Huffman(huf_b,q_nz);
                for(int i=0;i<q_z;i++)
                    HufCodeLength3[huf_sort[i].index]=0;
                for(int i=q_z;i<q_h;i++)
                    HufCodeLength3[huf_sort[i].index]=huf_b[i-q_z];
                huf_size+=HuffmanTableSize(HufCodeLength3,q_h);
                for(int i=0;i<q_nz;i++)
                    huf_size+=huf_b[i]*count3[i];
            }
            else// if(q_h==2)
            {
                if(count3[0]==0)
                    HufCodeLength3[0]=0;
                else
                    HufCodeLength3[0]=1;
                huf_size+=1+count3[0]+count3[1];
            }
            huf_size+=4;
            int a,b=1;
            for(int i=0;i<HQ;i++)
            {
                a=b;
//COMC.                b+=Group[i];
                b+=Group1[i];//COMC.
                huf_q[i]=0;
                for(int j=a;j<b;j++)
                    huf_q[i]+=count1[j];
            }
            int q_h;
            for(int i=HQ-1;i>=0;i--)
                if(huf_q[i]!=0)
                {
                    q_h=i+1;
                    break;
                }
            if(q_h>2)
            {
                for(int i=EB;i<q_h;i++)
                    huf_size+=huf_q[i]*ExtraBits[i];
                std::vector<SORT_HUFFMAN> huf_sort;
                SORT_HUFFMAN temp;
                for(int i=0;i<q_h;i++)
                {
                    temp.count=huf_q[i];
                    temp.index=i;
                    huf_sort.push_back(temp);
                }
                sort(huf_sort.begin(),huf_sort.end());
                int q_z;
                for(int i=0;i<q_h;i++)
                    if(huf_sort[i].count!=0)
                    {
                        q_z=i;
                        break;
                    }
                for(int i=q_z;i<q_h;i++)
                    huf_q[i-q_z]=huf_sort[i].count;
                int q_nz=q_h-q_z;
                uint32_t huf_b[HQ];
                for(int i=0;i<q_nz;i++)
                    huf_b[i]=huf_q[i];
                if(q_nz==1)
                    huf_b[0]=1;
                else
                    Huffman(huf_b,q_nz);
                for(int i=0;i<q_z;i++)
                    HufCodeLength1[huf_sort[i].index]=0;
                for(int i=q_z;i<q_h;i++)
                    HufCodeLength1[huf_sort[i].index]=huf_b[i-q_z];
                //  huf_sort.clear();
                huf_size+=HuffmanTableSize(HufCodeLength1,q_h);
                for(int i=0;i<q_nz;i++)
                    huf_size+=huf_b[i]*huf_q[i];
            }
            else if(q_h==2)//if(q_h!=1)
            {
                if(count[1]==0)
                    HufCodeLength1[0]=0;
                else
                    HufCodeLength1[0]=1;
                huf_size+=1+count1[1]+count1[2];
            }
            QH1=q_h;
        }
    }
    return huf_size;
}

void HufImage::Huffman(uint32_t A[],int n)
{
    int root;                  /* next root node to be used */
    int leaf;                  /* next leaf to be used */
    int next;                  /* next value to be assigned */
    int avbl;                  /* number of available nodes */
    int used;                  /* number of internal nodes */
    int dpth;                  /* current depth of leaves */
    /* check for pathological cases */
    if(n==0)
        return;
    if(n==1)
    {
        A[0]=0;
        return;
    }
    /* first pass, left to right, setting parent pointers */
    A[0]+=A[1];
    root=0;
    leaf=2;
    for(next=1;next<n-1;next++)
    {
        /* select first item for a pairing */
        if(leaf>=n || A[root]<A[leaf])
        {
            A[next]=A[root];
            A[root++]= next;
        }
        else
            A[next]=A[leaf++];
        /* add on the second item */
        if(leaf>=n || (root<next && A[root]<A[leaf]))
        {
            A[next]+=A[root];
            A[root++]=next;
        }
        else
            A[next]+=A[leaf++];
    }
    /* second pass, right to left, setting internal depths */
    A[n-2]=0;
    for(next=n-3;next>=0;next--)
        A[next]=A[A[next]]+1;
    /* third pass, right to left, setting leaf depths */
    avbl=1;
    used=dpth=0;
    root=n-2;
    next=n-1;
    while(avbl>0)
    {
        while(root>=0 && A[root]==(uint32_t)dpth)
        {
            used++;
            root--;
        }
        while(avbl>used)
        {
            A[next--]=dpth;
            avbl--;
        }
        avbl=used+used;
        dpth++;
        used=0;
    }
}

uint32_t HufImage::HuffmanTableSize(uint32_t* length,int q_h)
{
    uint32_t size=0;
    uint32_t q_h1=q_h-1;
    for(uint32_t i=0;i<q_h1;i++)
        size+=HUF[q_h][length[i]];
    return size;
}
void HufImage::WriteHuffmanTable(uint32_t* length,uint8_t q_h,uint32_t* huf_tree)
{
    int len;
    uint8_t q_h1=q_h-1;
    for(uint8_t i=0;i<q_h1;i++)
    {
        len=length[i];
        w_base=HUF[q_h][len];
        write(HUF_TABLE[q_h][len]);
    }
    //create Huffman tree
    if(q_h>2)
    {
        std::vector<SORT_HUFFMAN> huf_sort;
        SORT_HUFFMAN temp;
        for(uint32_t i=0;i<q_h;i++)
        {
            temp.count=length[i];
            temp.index=i;
            huf_sort.push_back(temp);
        }
        stable_sort(huf_sort.begin(),huf_sort.end());
        int tree[16]={0};
        int h=1;
        uint32_t code;
        for(uint32_t i=0;i<q_h;i++)
        {
            len=huf_sort[i].count;
            if(len!=0)
            {
                if(len!=h)
                    for(;h<len;h++)
                        tree[h]=0;
                code=tree[0]<<(h-1);
                for(int j=1;j<h;j++)
                    code+=tree[j]<<(h-j-1);
                huf_tree[huf_sort[i].index]=code;
                do
                {
                    if(tree[h-1]==0)
                    {
                        tree[h-1]=1;
                        break;
                    }
                    else
                        h--;
                }while(h!=0);
            }
        }
    }
}

void HufImage::ReadHuffmanTable(uint8_t* length,uint8_t q_h)
{
    HuffmanTree(HUF[q_h],q_h,TableTree);
    uint8_t q_h1=q_h-1;
    for(uint8_t i=0;i<q_h1;i++)
        ReadCode(length[i],TableTree);
    BitLength(length,q_h);
    ClearTree(TableTree);
}

void HufImage::HuffmanTree(uint8_t* length,uint8_t q_h,std::vector<Node*>& tree)
{
/*     if(tree.size())
         ClearTree(tree);*/
    std::vector<SORT_HUFFMAN> huf_sort;
    SORT_HUFFMAN h_temp;
    for(uint8_t i=0;i<q_h;i++)
    {
        h_temp.count=length[i];
        h_temp.index=i;
        huf_sort.push_back(h_temp);
    }
    stable_sort(huf_sort.begin(),huf_sort.end());
    Node* temp0=new Node(false);
    tree.push_back(temp0);
    uint8_t i=0;
    for(;i<q_h;i++)
        if(huf_sort[i].count!=0)
            break;
    Node* temp1;
    if(i!=q_h-1)
    {
        temp1=new Node(true);
        tree.push_back(temp1);
    }
    u_long j=0;
    Node* temp;
    u_long size;
    while(1)
    {
        size=tree.size();
        for(;j<size;j++)
        {
            temp=tree[j];
            if(temp->Gen==huf_sort[i].count)
            {
                temp->Leaf=true;
                temp->Value=huf_sort[i].index;
                i++;
            }
            else
                break;
        }
        if(j==size)
            break;
        for(u_long k=j;k<size;k++)
        {
            tree[k]->Divide(&temp0,&temp1);
            tree.push_back(temp0);
            tree.push_back(temp1);
        }
        j=size;
    }
}

void HufImage::ReadCode(uint8_t& value,std::vector<Node*>& tree)
{
    Node* node;
    bool bl;
    read(bl);
    if(bl)
        node=tree[1];
    else
        node=tree[0];
    while(!node->Leaf)
    {
        read(bl);
        if(bl)
            node=node->N1;
        else
            node=node->N0;
    }
    value=node->Value;
}
void HufImage::ReadCode(uint8_t q_huf,uint8_t& value,std::vector<Node*>& tree)
{
    bool bl;
    if(q_huf>2)
    {
        Node* node;
        read(bl);
        if(bl)
            node=tree[1];
        else
            node=tree[0];
        while(!node->Leaf)
        {
            read(bl);
            if(bl)
                node=node->N1;
            else
                node=node->N0;
        }
        value=node->Value;
    }
    else if(q_huf==2)
    {
        read(bl);
        if(bl)
            value=1;
        else
            value=0;
    }
    else
        value=0;
}
void HufImage::BitLength(uint8_t* length,uint8_t q_huf)
{
    uint8_t used[32];
    for(uint8_t i=0;i<q_huf;i++)
        used[i]=0;
    for(uint8_t i=0;i<q_huf-1;i++)
        used[length[i]]++;
    uint8_t q_h=q_huf-used[0];
    if(q_h==1)
    {
        length[q_huf-1]=1;
        return;
    }
    uint8_t pot[32];
    pot[1]=2;
    for(uint8_t i=1;i<q_huf;i++)
    {
        if(pot[i]==q_h && used[i]==pot[i]-1)
        {
            length[q_huf-1]=i;
            return;
        }
        if(pot[i]<q_h)
        {
            pot[i+1]=pot[i]+pot[i]-used[i]-used[i];
            q_h-=used[i];
        }
        else
        {
            uint8_t j=q_huf-1;
            for(;j>0;j--)
                if(used[j]!=0)
                    break;
            for(j--;j>0;j--)
            {
                used[j]+=used[j+1]/2;
                if(used[j]==1 || used[j]%2==1)
                {
                    length[q_huf-1]=j;
                    return;
                }
            }
        }
    }
}
void HufImage::ClearTrees()
{
    for(int z=0;z<3;z++) {
        if(QLHL[z]>2)
            ClearTree(LTree[z]);
    }
    if(QSH>2)
        ClearTree(STree);
    if(QH>2)
        ClearTree(Tree);
    if(QH1>2)
        ClearTree(Tree1);
    if(QH3>2)
        ClearTree(Tree3);
}
void HufImage::ClearTree(std::vector<Node*>& tree)
{
    u_long size=tree.size();
    for(u_long i=0;i<size;i++)
        delete tree[i];
    tree.clear();
}
//QH>2
void HufImage::ReadCompPixel1(uint8_t triple)
{
    uint8_t one;
    uint8_t min=ReadDelta(Tree);
    for(int k=0;k<3;k++)
    {
        Signs[k]=triple;
        Deltas[k]=min;
    }
    if(QH3>1)
    {
        ReadCode(QH3,one,Tree3);
        if(one)
        {
            for(int k=0;k<3;k++)
            {
                if((one>>k)%2)
                {
                    Deltas[k]+=(this->*ReaDe1)(Tree1);
                }
            }
        }
    }
}
//QH==2
void HufImage::ReadCompPixel1_2(uint8_t triple)
{
    uint8_t one;
    uint8_t min=ReadDelta_2(Tree);
    for(int k=0;k<3;k++)
    {
        Signs[k]=triple;
        Deltas[k]=min;
    }
    if(QH3>1)
    {
        ReadCode(QH3,one,Tree3);
        if(one)
        {
            for(int k=0;k<3;k++)
            {
                if((one>>k)%2)
                {
                    Deltas[k]+=1;//Deltas[k]=2
                }
            }
        }
    }
}
//QH==1
void HufImage::ReadCompPixel1_1(uint8_t triple)
{
    for(int k=0;k<3;k++)
    {
        Signs[k]=triple;
        Deltas[k]=1;
    }
}
void HufImage::ReadCompPixel2(uint8_t triple) {
    bool extra_bit;
    read(extra_bit);
    (this->*TriIn[triple])(extra_bit);
}
uint8_t HufImage::ReadDelta(std::vector<Node*> tree)
{
    uint8_t g_n;
    uint8_t rem;
    ReadCode(g_n,tree);
    if(ExtraBits[g_n]==0)
        return g_n+1;
    else
    {
        r_base=ExtraBits[g_n];
        read(rem);
        return Borders[g_n-1]+rem;
    }
}
uint8_t HufImage::ReadDelta_2(std::vector<Node*> tree)
{
    bool delta;
    read(delta);
    return delta?2:1;
}
uint8_t HufImage::ReadDelta_1(std::vector<Node*> tree)
{
    return 1;
}
/*
uint8_t HufImage::ReadDelta1()
{
    uint8_t g_n;
    uint8_t rem;
    if(ReadCode(g_n,Tree))
        return -1;
    if(ExtraBits[g_n]==0)
        return g_n+1;
    else
    {
        r_base=ExtraBits[g_n];
        read(rem);
        return Borders[g_n-1]+rem;
    }
}
uint8_t HufImage::ReadDelta1_2()
{
    bool delta;
    read(delta);
    return delta?2:1;
}
uint8_t HufImage::ReadDelta1_1()
{
    return 1;
}
 */
void HufImage::TripleInit3(bool extra_bit)
{
    if(extra_bit)
    {
        Signs[0]=1;
        Signs[1]=1;
        Signs[2]=0;
    }
    else
    {
        Signs[0]=0;
        Signs[1]=0;
        Signs[2]=1;
    }
    for(int k=0;k<3;k++)
    {
        Deltas[k]=(this->*ReaDe)(Tree);
    }
    Deltas+=3;
}
void HufImage::TripleInit4(bool extra_bit)
{
    if(extra_bit)
    {
        Signs[0]=1;
        Signs[1]=0;
        Signs[2]=1;
    }
    else
    {
        Signs[0]=0;
        Signs[1]=1;
        Signs[2]=0;
    }
    for(int k=0;k<3;k++)
    {
        Deltas[k]=(this->*ReaDe)(Tree);
    }
    Deltas+=3;
}
void HufImage::TripleInit5(bool extra_bit)
{
    if(extra_bit)
    {
        Signs[0]=0;
        Signs[1]=1;
        Signs[2]=1;
    }
    else
    {
        Signs[0]=1;
        Signs[1]=0;
        Signs[2]=0;
    }
    for(int k=0;k<3;k++)
    {
        Deltas[k]=(this->*ReaDe)(Tree);
    }
    Deltas+=3;
}
void HufImage::TripleInit6(bool extra_bit)
{
    Signs[0]=2;
    if(extra_bit)
    {
        Signs[1]=1;
        Signs[2]=1;
    }
    else
    {
        Signs[1]=0;
        Signs[2]=0;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas[1]=(this->*ReaDe)(Tree);
    Deltas+=2;
}
void HufImage::TripleInit7(bool extra_bit)
{
    Signs[1]=2;
    if(extra_bit)
    {
        Signs[0]=1;
        Signs[2]=1;
    }
    else
    {
        Signs[0]=0;
        Signs[2]=0;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas[1]=(this->*ReaDe)(Tree);
    Deltas+=2;
}
void HufImage::TripleInit8(bool extra_bit)
{
    Signs[2]=2;
    if(extra_bit)
    {
        Signs[0]=1;
        Signs[1]=1;
    }
    else
    {
        Signs[0]=0;
        Signs[1]=0;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas[1]=(this->*ReaDe)(Tree);
    Deltas+=2;
}
void HufImage::TripleInit9(bool extra_bit)
{
    Signs[0]=2;
    if(extra_bit)
    {
        Signs[1]=1;
        Signs[2]=0;
    }
    else
    {
        Signs[1]=0;
        Signs[2]=1;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas[1]=(this->*ReaDe)(Tree);
    Deltas+=2;
}
void HufImage::TripleInit10(bool extra_bit)
{
    Signs[1]=2;
    if(extra_bit)
    {
        Signs[0]=1;
        Signs[2]=0;
    }
    else
    {
        Signs[0]=0;
        Signs[2]=1;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas[1]=(this->*ReaDe)(Tree);
    Deltas+=2;
}
void HufImage::TripleInit11(bool extra_bit)
{
    Signs[2]=2;
    if(extra_bit)
    {
        Signs[0]=1;
        Signs[1]=0;
    }
    else
    {
        Signs[0]=0;
        Signs[1]=1;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas[1]=(this->*ReaDe)(Tree);
    Deltas+=2;
}
void HufImage::TripleInit12(bool extra_bit)
{
    Signs[0]=2;
    Signs[1]=2;
    if(extra_bit)
    {
        Signs[2]=1;
    }
    else
    {
        Signs[2]=0;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas++;
}
void HufImage::TripleInit13(bool extra_bit)
{
    Signs[0]=2;
    Signs[2]=2;
    if(extra_bit)
    {
        Signs[1]=1;
    }
    else
    {
        Signs[1]=0;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas++;
}
void HufImage::TripleInit14(bool extra_bit)
{
    Signs[1]=2;
    Signs[2]=2;
    if(extra_bit)
    {
        Signs[0]=1;
    }
    else
    {
        Signs[0]=0;
    }
    Deltas[0]=(this->*ReaDe)(Tree);
    Deltas++;
}

void HufImage::WriteCompSign(uint8_t triple,uint8_t s_gn,uint32_t *s_code_length)
{
    if(QSH>2) {
        w_base=s_code_length[s_gn];
        write(HufSTree[s_gn]);
    }
    else if(QSH==2) {
        write(s_gn==1);
    }
    if(triple%2)
        write(false);
    else
        write(true);
}
//QH>2
void HufImage::WriteCompDelta1(uint8_t min,uint32_t *code_length,uint32_t *code_length1,uint32_t *code3_length,uint8_t one)
{
    uint8_t gn;
    uint8_t rem;
    GroupNumber(min,gn,rem);
    w_base=code_length[gn];
    write(HufTree[gn]);
    if(ExtraBits[gn]!=0)
    {
        w_base=ExtraBits[gn];
        write(rem);
    }
    if(QH3>1)
    {
        if(QH3>2)
        {
            w_base=code3_length[one];
            write(HufTree3[one]);
        }
        else
            write((bool)one);
        if(one)
        {
            if(QH1>2) {
                for(int k=0;k<3;k++)
                {
                    if(DeltaCodes[DeltaOffset+k]!=0)
                    {
                        GroupNumber(DeltaCodes[DeltaOffset+k],gn,rem);
                        w_base=code_length1[gn];
                        write(HufTree1[gn]);
                        if(ExtraBits[gn]!=0)
                        {
                            w_base=ExtraBits[gn];
                            write(rem);
                        }
                    }
                }
            }
            else if(QH1==2) {
                for(int k=0;k<3;k++)
                {
                    if(DeltaCodes[DeltaOffset+k]!=0)
                    {
                        write(DeltaCodes[DeltaOffset+k]==2);
                    }
                }
            }
        }
    }
    DeltaOffset+=3;
}
//QH>2
/*
void HufImage::WriteCompDelta2(uint8_t triple,uint32_t *code_length)
{
    uint8_t gn;
    uint8_t rem;
    if(triple<=8)
    {
        for(int k=0;k<3;k++)
        {
            GroupNumber(DeltaCodes[DeltaOffset++],gn,rem);
            w_base=code_length[gn];
            write(HufTree[gn]);
            if(ExtraBits[gn]!=0)
            {
                w_base=ExtraBits[gn];
                write(rem);
            }
        }
    }
    else if(triple<=20) {
        for(int k=0;k<2;k++)
        {
            GroupNumber(DeltaCodes[DeltaOffset++],gn,rem);
            w_base=code_length[gn];
            write(HufTree[gn]);
            if(ExtraBits[gn]!=0)
            {
                w_base=ExtraBits[gn];
                write(rem);
            }
        }
    }
    else {
        GroupNumber(DeltaCodes[DeltaOffset++],gn,rem);
        w_base=code_length[gn];
        write(HufTree[gn]);
        if(ExtraBits[gn]!=0)
        {
            w_base=ExtraBits[gn];
            write(rem);
        }
    }
}
*/
void HufImage::WriteCompDelta2(uint8_t triple,uint32_t *code_length)
{
    uint8_t gn;
    uint8_t rem;
    GroupNumber(DeltaCodes[DeltaOffset++],gn,rem);
    w_base=code_length[gn];
    write(HufTree[gn]);
    if(ExtraBits[gn]!=0)
    {
        w_base=ExtraBits[gn];
        write(rem);
    }
    if(triple<=20) {
        GroupNumber(DeltaCodes[DeltaOffset++],gn,rem);
        w_base=code_length[gn];
        write(HufTree[gn]);
        if(ExtraBits[gn]!=0)
        {
            w_base=ExtraBits[gn];
            write(rem);
        }
    }
    if(triple<=8) {
        GroupNumber(DeltaCodes[DeltaOffset++],gn,rem);
        w_base=code_length[gn];
        write(HufTree[gn]);
        if(ExtraBits[gn]!=0)
        {
            w_base=ExtraBits[gn];
            write(rem);
        }
    }
}

//QH==2
void HufImage::WriteCompDelta1(uint8_t min,uint32_t *code3_length,uint8_t one)
{
    if(min==1)
    {
        write(false);
    }
    else// if(min==2)
    {
        write(true);
    }
    if(QH3>1)
    {
        if(QH3>2)
        {
            w_base=code3_length[one];
            write(HufTree3[one]);
        }
        else
            write((bool)one);
    }
    DeltaOffset+=3;
}
//QH==2
void HufImage::WriteCompDelta2(uint8_t triple)
{
    if(triple<=8)
    {
        for(int k=0;k<3;k++)
        {
            if(DeltaCodes[DeltaOffset++]==1)
            {
                write(false);
            }
            else
            {
                write(true);
            }
        }
    }
    else if(triple<=20) {
        for(int k=0;k<2;k++)
        {
            if(DeltaCodes[DeltaOffset++]==1)
            {
                write(false);
            }
            else
            {
                write(true);
            }
        }
    }
    else {
        if(DeltaCodes[DeltaOffset++]==1)
        {
            write(false);
        }
        else
        {
            write(true);
        }
    }
}
void HufImage::GroupNumber(uint8_t code,uint8_t& num,uint8_t& rem)
{
    for(uint8_t i=0;i<HQ;i++)
        if(code<Borders[i])
        {
            num=i;
            if(i>=EB)//6.
                rem=code-Borders[i-1];
            return;
        }
    num=-1;
}

void HufImage::GroupNumber1(uint32_t len,uint8_t& num,uint32_t& rem)
{
    for(uint8_t i=0;i<LHQ;i++)
        if(len<Borders1[i])
        {
            num=i;
            if(i!=0)
                rem=len-Borders1[i-1];
            return;
        }
    num=-1;
}

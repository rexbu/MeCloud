#include <stdio.h>
#include "m3u8.h"
#include "m3u8Clip.h"

using namespace std;

int main()
{
	M3U8Clip m3u8clip;
	m3u8clip.add("output.m3u8");
	m3u8clip.add("output1.m3u8");
	/*m3u8clip.add("output4.m3u8");
	//m3u8clip.add("output3.m3u8");
	m3u8clip.add("output5.m3u8");
	m3u8clip.add("output4.m3u8");
	m3u8clip.add("output4.m3u8");
	//cout<<"m3u8clip.add(\"output5.m3u8\",4):"<<m3u8clip.add("output5.m3u8",3)<<endl;
	//m3u8clip.selectAll();
	
	m3u8clip.display();
	//m3u8clip.removeTS(0);
	//m3u8clip.display();
	//m3u8clip.remove("output4.m3u8");
	//cout<<m3u8clip.remove(3)<<endl;
	//m3u8clip.display();*/
	m3u8clip.selectTS(2,5);
	//cout<< m3u8clip.targetduration<<endl;
	//string m3u8file = getname();
	m3u8clip.build(".");
	
	return 0;
}

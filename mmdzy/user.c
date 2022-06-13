#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

#define PRINT "/dev/DZYPLUS"      //设备文件名称及路径

int main(){
	int _open,_read,_ioctl;
	
	int cmd,flag;

	_open = open(PRINT,O_RDWR);
	if(_open < 0) {
		printf("%s open failed\n",PRINT);
		return 0;
	}

	printf("%s open success\n",PRINT);

	flag = 1;
	while(1){
		if(flag == 0)break;
		printf("cmd=0:exit\n");
		printf("cmd=101:start 1-100(1s) print\n");
		printf("cmd=102:pause 1-100(1s) print\n");
		printf("cmd=103:continue 1-100(1s) print\n");
		printf("cmd=104:stop 1-100(1s) print\n");
		printf("cmd=201:start 1-100(2s) print\n");
                printf("cmd=202:pause 1-100(2s) print\n");
                printf("cmd=203:continue 1-100(2s) print\n");
                printf("cmd=204:stop 1-100(2s) print\n");
		printf("cmd=901:start 1-100(1s),1-100(2s) print\n");
                printf("cmd=902:pause 1-100(1s),1-100(2s) print\n");
                printf("cmd=903:continue 1-100(1s),1-100(2s) print\n");
                printf("cmd=904:stop 1-100(1s),1-100(2s) print\n");
		printf("input cmd(zifu): ");
		scanf("%d",&cmd);
		switch(cmd){
			case 0:
				flag = 0;
				break;
			case 101://start
				_ioctl = ioctl(_open,101,0);
				if(_ioctl < 0)printf("thread1 has already started\n");
				else printf("start thread1 success\n");
				break;
			case 102://pause
				 _ioctl = ioctl(_open,102,0);
				 printf("pause thread1 success\n");
                                break;
			case 103://continue
				 _ioctl = ioctl(_open,103,0);
                                if(_ioctl < 0)printf("continue thread1 failed\n");
                                else printf("continue thread1 success\n");
                                break;
			case 104://stop
				 _ioctl = ioctl(_open,104,0);
                                printf("stop thread1 success\n");
                                break;
			case 201:
                                _ioctl = ioctl(_open,201,0);
                                if(_ioctl < 0)printf("thread2 has already started\n");
                                else printf("start thread2 success\n");
                                break;
                        case 202:
                                 _ioctl = ioctl(_open,202,0);
                                 printf("pause thread2 success\n");
                                break;
                        case 203:
                                 _ioctl = ioctl(_open,203,0);
                                if(_ioctl < 0)printf("continue thread2 failed\n");
                                else printf("continue thread2 success\n");
                                break;
			case 204:
                                 _ioctl = ioctl(_open,204,0);
                                printf("stop thread2 success\n");
                                break;
			case 901:
                                _ioctl = ioctl(_open,901,0);
                                if(_ioctl == -1)printf("two threads have already started\n");
                                if(_ioctl == 1)printf("start thread1 success,thread2 has already started\n");
				if(_ioctl ==2) printf("start thread2 success,thread1 has already started\n");
				if(_ioctl == 0)printf("start two threads success\n");
                                break;
                        case 902:
                                 _ioctl = ioctl(_open,902,0);
                                printf("pause two threads success\n");
                                break;
                        case 903:
                                 _ioctl = ioctl(_open,903,0);
                                if(_ioctl == -1)printf("two threads have already continued\n");
                                if(_ioctl == 1)printf("continue thread1 success,thread2 has already continued\n");
                                if(_ioctl ==2) printf("continue thread2 success,thread1 has already continued\n");
                                if(_ioctl == 0)printf("continue two threads success\n");

                                break;
                        case 904:
                                 _ioctl = ioctl(_open,904,0);
                                printf("stop two threads success\n");
                                break;
			default:
				break;
		}
	}

	close(_open);
	return 0;
}



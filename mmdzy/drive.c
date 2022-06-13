#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/fs.h>
#include<linux/fcntl.h>
#include<linux/ctype.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/vmalloc.h>
#include<linux/pagemap.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/kthread.h>
#include<linux/timer.h>
#include<linux/sched.h>
#include<linux/ktime.h>
#include<linux/jiffies.h>
#include<linux/semaphore.h>


#define HAO 16 		 //字符设备号为16
#define TNAME "GR"  	 //设备名GR
#define LOGO "GGG"  	 //LOGO
#define RANGE 50//打印范围

#define CLOCK1 jiffies + HZ
#define CLOCK2 jiffies + HZ * 2    //设定到期时间

struct semaphore signal1,signal2;     

struct timer_list time1;  //time1是一个结构体变量，为实现间隔1S打印     定时器
struct timer_list time2;  //time2是一个结构体变量，为实现间隔2S打印

struct task_struct *thread1;     //thread1
struct task_struct *thread2;     //thread2


int output1,output2;             //需要复位   全局变量output1 1-100（间隔1s）

void _time_1(void){
	printk("%s_THREAD1: %d\n",LOGO,output1);
	up(&signal1);
}
void _time_2(void){
	 printk("%s_THREAD2: %d\n",LOGO,output2);
         up(&signal2);
}

//初始化并启动定时器time1
void init_time1(void){
     timer_setup(&time1,(void*)_time_1,0);
     time1.expires = CLOCK1;
     add_timer(&time1);
}

void init_time2(void){
     timer_setup(&time2,(void*)_time_2,0);
     time2.expires = CLOCK2;
     add_timer(&time2);
}

static void thread1_1(void){
	output1=1;
	down(&signal1);
	for(output1=2;output1<=RANGE;output1+=1){
		mod_timer(&time1,CLOCK1);
		down(&signal1);
	}
}

static void thread2_1(void){
        output2=1;
	 down(&signal2);
        for(output2=2;output2<=RANGE;output2+=1){
                mod_timer(&time1,CLOCK2);
                down(&signal2);
        }

}


//open 设备文件    filp文件 
static int drive_open(struct inode *node,struct file *filp){
	sema_init(&signal1,0);
	sema_init(&signal2,0);
	thread1 = kthread_create((void*)thread1_1,NULL,"thread1");
	wake_up_process(thread1);

	thread2 = kthread_create((void*)thread2_1,NULL,"thread2");
        wake_up_process(thread2);

	return 0;
}

//接收用户输入的命令调用对应函数  打开的设备文件    命令          
static long drive_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	switch(cmd) {
		case 101:
			if(timer_pending(&time1)) return -1;
			else{
			init_time1();
			return 0;}
			break;              //线程1开始
		case 102:
			del_timer(&time1);
			return 0;	//线程1暂停
			break;
		case 103:
			 if(timer_pending(&time1)) return -1;
                        else{
                        init_time1();
                        return 0;}
			break;               //线程1继续
		case 104:
			del_timer(&time1);
			output1=1;
			return 0;
			break;               //线程1结束
		case 201:
                         if(timer_pending(&time2)) return -1;
                        else{
                        init_time2();
                        return 0;}
                        break;              //线程2开始
                case 202:
                        del_timer(&time2);   //线程2暂停
                        return 0;
			break;
                case 203:
                         if(timer_pending(&time2)) return -1;
                        else{
                        init_time2();
                        return 0;}
                        break;               //线程2继续
		case 204:
                        del_timer(&time2);
                       output2 =1;
		       return 0;
                        break;

		 case 901:
			if(timer_pending(&time1) && timer_pending(&time2)) 
				return -1;
			if(timer_pending(&time1) && !timer_pending(&time2)){
                        init_time2();return 2;}
			if(!timer_pending(&time1) && timer_pending(&time2)){
			init_time1();return 1;}
			if(!timer_pending(&time1) && !timer_pending(&time2)){
                        init_time1();init_time2();return 0;}

                        break;              //线程1,2开始
                case 902:
                        del_timer(&time1);   //线程1,2暂停
			del_timer(&time2);
			return 0;
                        break;
                case 903:
                         if(timer_pending(&time1) && timer_pending(&time2))
                                return -1;
                        if(timer_pending(&time1) && !timer_pending(&time2)){
                        init_time2();return 2;}
                        if(!timer_pending(&time1) && timer_pending(&time2)){
                        init_time1();return 1;}
                        if(!timer_pending(&time1) && !timer_pending(&time2)){
                        init_time1();init_time2();return 0;}

                        break;               //线程1,2继续
                case 904:
                        del_timer(&time1);
                        output1=1;
			del_timer(&time2);
			output2=1;
                        break;
		default:
			break;
	}
	return 0;
}


//结构体：本驱动程序使用的函数
static struct file_operations func_s = {
	.open = drive_open,
	.unlocked_ioctl = drive_ioctl
};



//init初始化  insmod会运行
static int __init drive_init(void){
	int _init = register_chrdev(HAO,TNAME,&func_s);//注册设备名和设备号   _init接收成功与否
	if(_init < 0){
		printk("%s:MMG failed!!!\n",LOGO);
	}
	else {
		printk("%s:MMG success!!!\n",LOGO);
	}
	return 0;
}

//exit退出  rmmod会运行
static void __exit drive_exit(void){
	unregister_chrdev(HAO,TNAME);
	printk("%s:unregister success!\n",LOGO);
}

module_init(drive_init);
module_exit(drive_exit);

MODULE_AUTHOR("TMM");
MODULE_LICENSE("GPL");   //许可
MODULE_DESCRIPTION("PRINT 1-100");  //目的

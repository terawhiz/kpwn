#include <linux/init.h>   /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/fs.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ex4722");
MODULE_DESCRIPTION("PWN ME :)");


#define proc_name "Sofire"
#define sofirium_art "  :!J5PPP5J!:\n 75PPPP55PPP57\nJPPPP!:!!JPPPPJ\nPPPPP!^~~7PPPPP\nJPPPP7!!:~PPPPJ\n 75PPP55PPPP57\n  :!J5PPP5J!:\n"
#define CHUNK_SIZE 0x100

#define DEV_READ   0xcafebabe
#define DEV_WRITE  0xbabecafe
#define DEV_CREATE 0xdeadbeef
#define DEV_DELETE 0X1337

#define debug_print(...)    do { if ( verbose) {printk(__VA_ARGS__);} \
} while (0)


int Major_num;
int verbose = 1;

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *filp, struct file *file);
int device_release(struct inode *inode, struct file *file);
long device_ioctl(struct file *, unsigned int, unsigned long);

struct file_operations fops = {
    .open = device_open,
    .unlocked_ioctl = device_ioctl,
    .compat_ioctl = device_ioctl,
    .release = device_release,
};

typedef struct sofirium_head{
    char coin_art[0x70];
    struct sofirium_entry* head;
    int total_nft;
} sofirium_head;

typedef struct sofirium_entry{
    struct sofirium_entry* next;
    char nft[CHUNK_SIZE];
} sofirium_entry;

typedef struct request{
    int idx;
    char buffer[CHUNK_SIZE];
} request;

sofirium_head * head;

int init_module(void) {
    Major_num = register_chrdev(0, proc_name, &fops);
    if (Major_num < 0) {
        printk(KERN_INFO "Failed to register device, major num returned %d",
                Major_num);
        return Major_num;
    }
    printk(KERN_INFO "Sucessfully registered device, major num returned %d", Major_num);
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", proc_name, Major_num);

    printk(KERN_INFO "Welcome to Sofirium, the greatest blockchain to exist");
    return 0;
}

void cleanup_module(void){ 
    printk(KERN_INFO "Sad to See you leave :(");
    unregister_chrdev(Major_num, proc_name);
}

static int device_open(struct inode *inode_num, struct file *file) {
    printk(KERN_INFO "Sofirium Device Opened");
    file->private_data = ((void *)0);
    return 0;
}

int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Sofirium Device Released");
    return 1;
}

long device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    sofirium_entry* next;
    sofirium_entry* new;
    sofirium_entry* target;
    sofirium_entry* tmp;
    request req;
    int total_nft;

    if (copy_from_user(&req, (void*)arg, sizeof(request))) {
        printk(KERN_INFO "Copy Request from User Error");
        return -EFAULT;
    }

    switch (cmd) {
        case DEV_DELETE:
            debug_print(KERN_INFO "Deleting Blockchain: Sofirium is Bad");

            next = head->head;
            total_nft= head->total_nft;
            kfree(head);

            for (int i = 0; i < total_nft; i ++){
                debug_print(KERN_INFO "Freeing Buffer 0x%px\nNEXT: 0x%px", tmp, next->next);
                tmp = next;
                next = next->next;
                kfree(tmp);
            }

            return 1;

        case DEV_CREATE:

            if (head == NULL){
                head = kmalloc(sizeof(sofirium_head), GFP_KERNEL);
                head->total_nft = 0;
                strlcpy(head->coin_art, sofirium_art, sizeof(head->coin_art));

                printk(KERN_INFO "%s", head->coin_art);

                head->head = NULL;
                debug_print(KERN_INFO "Head NULL, Creating sofirium_head at 0x%px", head);
            }

            if (head->total_nft == 0){
                new = kmalloc(sizeof(sofirium_entry), GFP_KERNEL);
                new->next = NULL;
                memcpy(new->nft, req.buffer, CHUNK_SIZE);
                head->head = new;
                head->total_nft = 1; 
            } else {
                target = head->head;
                for (int i=1; i < head->total_nft; i++){
                    target = target->next;
                }
                new = kmalloc(sizeof(sofirium_entry), GFP_KERNEL);
                new->next = NULL;
                memcpy(new->nft, req.buffer, CHUNK_SIZE);
                target->next = new;
                head->total_nft ++;
            }

            debug_print(KERN_INFO "NEW NFT: %s @ 0x%px \n",new->nft, new);
            return head->total_nft;

        case DEV_READ:
            target = head->head;
            for (int i=0; i < req.idx; i++){
                debug_print(KERN_INFO "Walked over entry 0x%px", target->next);
                target = target->next;
            };



            debug_print(KERN_INFO "Copy to user %s @ 0x%px", target->nft, target->nft);
            if(copy_to_user((void*)arg+offsetof(struct request, buffer),target->nft, sizeof(target->nft))){
                printk(KERN_INFO "Copy to user failed, exiting");
                return -EFAULT;
            }
            return 0;

        case DEV_WRITE:
            target = head->head;
            for (int i=0; i < req.idx; i++){
                debug_print(KERN_INFO "Walked over entry %px", target->next);
                target = target->next;
            };

            if(copy_from_user(target->nft, (void*)arg+offsetof(struct request, buffer),sizeof(target->nft))){
                printk(KERN_INFO "Copy from user failed exiting");
                return -EFAULT;
            }
            debug_print(KERN_INFO "Copy from user %s to 0x%px", target->nft, target->nft);

            return 0;
        default:
            return 0xffff;
    }
}


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/cdev.h>

static dev_t dev;
static struct cdev hello_cdev;
static const unsigned int count = 1;

static const struct file_operations hello_fops = {
    .owner = THIS_MODULE,
};

static int __init hello_init(void)
{
     int res;

    res = alloc_chrdev_region(&dev, 0, count, "hello_device");


    if (res < 0) {
         pr_err("alloc_chrdev_region failed: %d\n", res);
        return res;
    }

    cdev_init(&hello_cdev, &hello_fops);
    hello_cdev.owner = THIS_MODULE;

    res = cdev_add(&hello_cdev, dev, count);
    if (res < 0) {
        pr_err("cdev_add failed: %d\n", res);
        kobject_put(&hello_cdev.kobj);
        unregister_chrdev_region(dev, count);
        return res;
    }

    printk(KERN_INFO "hello world\n");
    return 0;
}
static void __exit hello_exit(void)
{
    cdev_del(&hello_cdev);
    unregister_chrdev_region(dev, count);
    printk(KERN_INFO "bye bye world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Printk exercise: messages on load and unload");

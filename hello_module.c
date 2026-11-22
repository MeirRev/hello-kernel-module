#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>

int res;
static dev_t dev;
static struct cdev hello_cdev;
struct class *hello_class;
struct device * hello_device;
static const unsigned int count = 1;

static const struct file_operations hello_fops = {
    .owner = THIS_MODULE,
};

static int __init hello_init(void)
{
    
    

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
        goto err_unregister_region;
    }
    hello_class = class_create("hello_class");

    if (IS_ERR(hello_class)) {
        res =PTR_ERR(hello_class);
        goto err_del_cdev;
    }
    hello_device = device_create (hello_class,NULL,dev,NULL,"hello_device%d",count -1);
    if (IS_ERR(hello_device)) {
        res = PTR_ERR(hello_device);
        goto  err_destroy_class;
    }

    printk(KERN_INFO "hello world\n");
    return 0;

    

    err_destroy_class:
        class_destroy(hello_class);

    err_del_cdev:
        cdev_del(&hello_cdev);

    err_unregister_region:
        unregister_chrdev_region(dev, count);
        return res;
}
static void __exit hello_exit(void)
{
    device_destroy(hello_class,dev);
    class_destroy(hello_class);
    cdev_del(&hello_cdev);
    unregister_chrdev_region(dev, count);
    printk(KERN_INFO "bye bye world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Printk exercise: messages on load and unload");





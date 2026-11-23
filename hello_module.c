#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/string.h>

int res;
static dev_t dev;
static struct cdev hello_cdev;
struct class *hello_class;
struct device * hello_device;
static const unsigned int count = 1;

struct hello_device_state {
    atomic_t array_count[2];
    char data[256];
    size_t data_len;
};

static struct hello_device_state hello_state = {
    .array_count = { ATOMIC_INIT(0), ATOMIC_INIT(0) },
};
static DEFINE_MUTEX(buffer_lock);



static int my_open(struct inode *ino, struct file *filp)
{
    atomic_t *counts = hello_state.array_count;
    filp->private_data = &hello_state;
    atomic_inc(&counts[0]);
    printk(KERN_INFO "hello_device: opens=%d\n", atomic_read(&counts[0]));
    return 0;


}

static int my_release(struct inode *ino, struct file *file)
{
    struct hello_device_state *priv = file->private_data;
    atomic_t *counts = priv->array_count;
    atomic_inc(&counts[1]);
    printk(KERN_INFO "hello_device: releases=%d\n", atomic_read(&counts[1]));
    printk(KERN_INFO "hello_device: activities=%d\n",atomic_read(&counts[0])- atomic_read(&counts[1]));
    return 0;

}

static ssize_t my_read(struct file *filp,char __user *buff,size_t count,loff_t *offp)
{
    size_t available;
    size_t n;
    ssize_t result = 0;
    struct hello_device_state *priv = filp->private_data;

    if (*offp < 0){
        pr_err("location no correct: %d\n", EINVAL);
        return -EINVAL;
    }

    mutex_lock(&buffer_lock);
    if (*offp >= (loff_t)priv->data_len)
        goto out_unlock;

    available = priv->data_len - (size_t)*offp;
    n = count < available ? count : available;

    if (copy_to_user(buff, priv->data + *offp, n) != 0) {
        result = -EFAULT;
        goto out_unlock;
    }
    *offp += n;
    result = n ;
    out_unlock:
        mutex_unlock(&buffer_lock);
        return result;
}

static ssize_t my_write(struct file *filp, const char __user *buff,size_t count, loff_t *offp){

    struct hello_device_state *priv = filp->private_data;
    char tmp[sizeof(priv->data)];
    size_t available;
    size_t n;
    ssize_t result;

    if (*offp < 0){
        pr_err("location no correct: %d\n", EINVAL);
        return -EINVAL;
    }

    if (count == 0)
        return 0;

     mutex_lock(&buffer_lock);

    if (*offp >= (loff_t)sizeof(priv->data)) {
        result = -ENOSPC;
        goto out_unlock;
    }

    available = sizeof(priv->data) - (size_t)*offp;
    n = count < available ? count : available;

    if (copy_from_user(tmp, buff, n) != 0) {
        result = -EFAULT;
        goto out_unlock;
    }

    memcpy(priv->data + *offp, tmp, n);
    *offp += n;

    if ((size_t)*offp > priv->data_len)
        priv->data_len = (size_t)*offp;

    result = n;

out_unlock:
    mutex_unlock(&buffer_lock);
    return result;

}


static const struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
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





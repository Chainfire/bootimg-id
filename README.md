Reads/writes the ID of a boot image

License: whatever

Also see: http://androidxref.com/6.0.1_r10/xref/system/core/mkbootimg/bootimg.h

Example usage on an HTC10:

```
bootimg-id /dev/block/platform/soc/*/by-name/recovery $(getprop.ro.expect.recovery_id)
```


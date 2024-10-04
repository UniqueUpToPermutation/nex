Package: rttr:x64-linux@0.9.6+20210811#1

**Host Environment**

- Host: x64-linux
- Compiler: GNU 11.4.0
-    vcpkg-tool version: 2024-04-23-d6945642ee5c3076addd1a42c331bbf4cfc97457
    vcpkg-scripts version: 7eb700c96 2024-05-17 (5 months ago)

**To Reproduce**

`vcpkg install --allow-unsupported`

**Failure logs**

```
-- Downloading https://github.com/rttrorg/rttr/archive/7edbd580cfad509a3253c733e70144e36f02ecd4.tar.gz -> rttrorg-rttr-7edbd580cfad509a3253c733e70144e36f02ecd4.tar.gz...
-- Extracting source /home/phil/vcpkg/downloads/rttrorg-rttr-7edbd580cfad509a3253c733e70144e36f02ecd4.tar.gz
-- Applying patch fix-directory-output.patch
-- Applying patch Fix-depends.patch
-- Applying patch remove-owner-read-perms.patch
-- Applying patch disable-unsupport-header.patch
-- Applying patch disable-werrorr.patch
-- Using source at /home/phil/vcpkg/buildtrees/rttr/src/e36f02ecd4-2c5f77cdfa.clean
CMake Error at /home/phil/.cache/vcpkg/registries/git-trees/942efa7d91154b3d9841608a015b1c216ca0f251/portfile.cmake:28 (message):
  rttr's build system does not support this configuration:
  VCPKG_LIBRARY_LINKAGE: static VCPKG_CRT_LINKAGE: dynamic
Call Stack (most recent call first):
  scripts/ports.cmake:175 (include)



```

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "dependencies": [
    "expected-lite",
    "glfw3",
    "glm",
    "plog",
    "rttr"
  ]
}

```
</details>

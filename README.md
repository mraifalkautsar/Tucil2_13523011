# Kompres Dulu Pake QuadTree!

![smallsize_SSIM](https://github.com/user-attachments/assets/f8c2571a-b57f-41d9-af80-95c202b5b64e)

## 📷 Deskripsi Singkat

Program ini mengkompresi gambar dengan metode **Quadtree**, sebuah struktur data hierarkis yang sering digunakan dalam pemrosesan citra. Quadtree membagi gambar menjadi blok-blok kecil berdasarkan keseragaman warna piksel (dalam format RGB). 

Jika blok tidak seragam, maka akan dibagi lagi menjadi empat sub-blok hingga memenuhi ambang batas keseragaman atau mencapai ukuran minimum yang ditentukan.

## 🔧 Cara Kompilasi

Jalankan perintah berikut dari direktori root untuk mengompilasi program:
``` g++ -o ./bin/main ./src/main.cpp ./src/quadtree.cpp ./src/quadtreenode.cpp ./src/utils.cpp ```
> Pastikan Anda menggunakan **g++** dan compiler mendukung C++17 atau lebih baru.
> 
## ▶️ Cara Menjalankan

Setelah proses kompilasi selesai, jalankan program dengan:

``` ./bin/main ```

Program akan meminta path gambar input dan parameter lainnya.

## 👨‍💻 Pembuat

| Nama                     | NIM       |
|--------------------------|-----------|
| Muhammad Ra'if Alkautsar | 13523011  |

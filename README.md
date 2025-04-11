# Kompres Dulu Pake QuadTree!


<p align="center">
  <img src="https://github.com/user-attachments/assets/c0d8c441-8a26-4711-9bb7-79e9f871caf7" alt="mediumsize_SSIM"/>
</p>

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

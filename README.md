
# Reverse-Delete Algoritmasının Etkileşimli Bir Uygulaması ve Görselleştirilmesi

Karadeniz Teknik Üniversitesi Bilgisayar Mühendisliği Bölümü Ayrık Matematik Dersi Yarıyıl Sonu Serbest Ödevi

| | | 
| ---------------|------------------------ |
| Hazırlayan     | Deniz Tunç - 434377     |
| Ödev Sorumlusu | Doç. Dr. Vasıf Nabiyev  |

## Projenin Amacı

Bu projenin amacı Reverse-Delete algoritmasının adımlarının öğrenciler tarafından daha kolay kavranmasıdır. Proje, bir graf düzenleyici içermektedir. Algoritma herhangi bir anda durdurulma ve tekrar başlatılma özelliğine sahiptir.


## Yöntem

Proje, C programlama dilinde yazılmıştır. C programlama dili, etkileşimli programlar konusunda C++, JavaScript ve C# gibi dillere göre çok tercih edilmemektedir. Ancak bence basit yapısı ve Web Assembly teknolojisi ile internet ortamına aktarılabilir olması, C dilini bu proje için oldukça ideal kılmaktadır.

Grafik ve arayüz işlemleri için minimal bir yapıya sahip olan [Raylib](#3) kütüphanesi kullanılmıştır. Raylib, kendisi de C dilinde yazılmış olmakla birlikte, çok temel bir API'a sahiptir. Kütüphane pencere oluşturma, girdi aygıtlarını kontrol etme ve ekrana temel şekiller çizme gibi işlemleri üstenmekte ancak çoğu oyun motorunun aksine gereksiz yere kaynak kullanımına neden olmamaktadır. Proje yazılırken Raylib v5.0 sürümü kullanılmıştır.

Algoritmanın etkileşimli olması için gerektiğinde durdurulması ve devam ettirilmesi gerekmektedir. Bu amaçla proje, Pthread (POSIX Thread) kütüphanesini kullanarak multi threading yapmaktadır.

Graflar ve algoritma ile ilgili yapıların tamamı sıfırdan yazılmıştır. Graf yapısında tutulan bilgiler düğüm sayısı ve komşuluk matrisinden oluşmaktadır. Algoritma için gerekli diğer tüm bilgiler bu iki değerden türetilebilir. Komşuluk matrisi aynı zamanda kenar ağırlıklarını tutmaktadır.

## Algoritma Hakkında

Reverse-Delete Algoritması, herhangi bir bağlı grafta **Minimum Açılım Ağacının (MST)** bulmaya çalışan sezgisel bir algoritmadır. Algoritma ilk olarak [Kruskal (1956)](#1)'da ortaya konulmaktadır ve yine aynı makalede ortaya atılan [Kruskal Algoritması](#2)'nın işlemsel olarak tersidir. 

Algoritmanın işleyişinin ana hatları aşağıdaki gibidir:

1. Graftaki kenarlar leksografik olarak ağırlığına göre büyükten küçüğe sıralanır.
2. Her *e* kenarı için sırasıyla:
3.  *e* köprü oluşturuyorsa, bir sonraki kenara atlanır.
4.  Aksi takdirde *e* graftan çıkarılır.

Bu adımlardan 2. adımdaki döngü, açılım ağaçları da bir ağaç türü olduğundan, ağaçların özelliklerinden $|V|=|E|-1$ özelliğini kullanarak erken çıkış yapabilir. Program bunu göz önünde bulundurmaktadır.

Algoritmanın 3. adımındaki köprü oluşturma kontrolü programda biraz farklı bir şeklide gerçekleştirilir. Program, önce köprü koşulunu kontrol etmek yerine *e* kenarını kaldırır. Sonra *Depth First Search* kullanarak grafın bağlı kalıp kalmadığını kontrol eder ve eğer graf bağlı değilse *e* grafa geri eklenir. Programın bu kısmı, döngü matrislerinin hesaplanması ile geliştirilmeye açıktır.

## Karşılaşılan zorluklar

C dilininde bellekte yer ayrılması için programcının açık bir şekilde ayrılacak yer miktarını belirtmesi gerekmektedir ve çalışma zamanında bellekle ilgili işlevlerin doğruluğunu kontrol eden bir araç varsayılan olarak bulunmamaktadır. Bu da bazı hataların kaynağının tespit edilmesini güç kılmaktadır.
Projenin etkileşimli hale getirilmesi sırasında, yanlış bellek tahsisi ve out-of-bounds erişim gibi hatalar, komşuluk matrisi yapısının yeniden boyutlandırması ve değiştirilmesi durumlarında yaygın olarak ortaya çıktı. Hataların kaynağının tespiti için GCC derleyicisinin `-fsanitize=address` ve `-g` seçenekleri kullanıldı [<sup>4</sup>](#4).

## Kaynak kodu ve belgelendirme

Programın kaynak kodu https://github.com/creeperkafasi/graph-algo.git git deposu içerisinde saklanmaktadır. Projede bulunan çoğu fonksiyonun açıklaması yorum satırı şeklinde verilmiştir.

Kod, MIT Açık Kaynak Lisansı ile paylaşılmıştır. Lisansın bir kopyası, kaynak kodunun bulunduğu depoda bulunmaktadır.

## Kaynaklar ve Bağlantılar

1. <a id="1" href="https://www.ams.org/journals/proc/1956-007-01/S0002-9939-1956-0078686-7/?active=current">Kruskal, Joseph B. "On the shortest spanning subtree of a graph and the traveling salesman problem." Proceedings of the American Mathematical Society, 1956.</a>
2.  <a id="2" href="https://en.wikipedia.org/wiki/Kruskal%27s_algorithm">Kruskal's algorithm - Wikipedia</a> 
3.  <a id="3" href="https://www.raylib.com/">raylib | A simple and easy-to-use library to enjoy videogames programming</a> 
4.  <a id="4" href="https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#index-fsanitize_003daddress">Instrumentation Options (Using the GNU Compiler Collection (GCC))</a>

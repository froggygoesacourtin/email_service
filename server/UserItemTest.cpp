#include "UserItemStore.h"

int main() {
 UserItemStore store;
 int user, item;

 store.add(10, 100);
 store.add(10, 200);

 store.add(20, 100);
 store.add(20, 200);
 store.add(20, 300);

 printf("step 1: user size: %d item size: %d\n", store.user_size(), store.item_size());

 store.remove(10, 100);
 store.remove(10, 200);

 printf("step 2: user size: %d item size: %d\n", store.user_size(), store.item_size());

 store.clear();

 FILE *fp = fopen("UserItemStoreTestGen.dat", "r");
 while (fscanf(fp, "%d %d", &user, &item) != EOF) {
  store.add(user, item);
 }

 store.createDist();

 store.createItemClusters();
 store.print(store._itemClusterMap);

 store.write();

 return 0;
}

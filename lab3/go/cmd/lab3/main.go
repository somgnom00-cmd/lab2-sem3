package main

import (
	"fmt"
	"lab3/structures"
)

func printContainer(container structures.Container) {
	fmt.Printf("%s %v\n", container.Type(), container.Values())
}

func main() {
	fmt.Println("LAB3, variant 1. Go demo")

	array := structures.NewArray()
	array.PushBack("alpha")
	array.PushBack("gamma")
	_ = array.Insert(1, "beta")
	_ = array.Erase(0)
	printContainer(array)

	tree := structures.NewCompleteBinaryTree()
	for value := 1; value <= 7; value++ {
		tree.Insert(fmt.Sprint(value))
	}
	printContainer(tree)
	fmt.Printf("Tree is complete: %v\n", tree.IsComplete())

	linear := structures.NewLinearHashTable(17)
	doubled := structures.NewDoubleHashTable(17)
	for _, key := range []int{1, 1001, 2001} {
		linear.Set(key, key*10)
		doubled.Set(key, key*10)
	}
	printContainer(linear)
	printContainer(doubled)
	fmt.Println("Demo completed successfully.")
}

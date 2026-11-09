package structures

import (
	"path/filepath"
	"reflect"
	"strconv"
	"testing"
)

func TestArray(t *testing.T) {
	array := NewArray()
	array.PushBack("a")
	array.PushBack("c")
	if err := array.Insert(1, "b"); err != nil {
		t.Fatal(err)
	}
	if err := array.Erase(0); err != nil {
		t.Fatal(err)
	}
	if !reflect.DeepEqual(array.Values(), []string{"b", "c"}) {
		t.Fatalf("unexpected values: %v", array.Values())
	}
	if _, err := array.At(9); err == nil {
		t.Fatal("out-of-range access must return an error")
	}
}

func TestLists(t *testing.T) {
	singly := NewSinglyList()
	singly.PushBack("a")
	singly.PushBack("c")
	if err := singly.Insert(1, "b"); err != nil {
		t.Fatal(err)
	}
	if err := singly.Erase(2); err != nil {
		t.Fatal(err)
	}
	if !reflect.DeepEqual(singly.Values(), []string{"a", "b"}) {
		t.Fatalf("unexpected singly list: %v", singly.Values())
	}

	doubly := NewDoublyList()
	doubly.PushFront("b")
	doubly.PushFront("a")
	doubly.PushBack("c")
	if !reflect.DeepEqual(doubly.ReverseValues(), []string{"c", "b", "a"}) {
		t.Fatalf("unexpected reverse list: %v", doubly.ReverseValues())
	}
}

func TestStackAndQueue(t *testing.T) {
	stack := NewStack()
	stack.Push("first")
	stack.Push("second")
	if value, _ := stack.Pop(); value != "second" {
		t.Fatalf("stack is not LIFO: %q", value)
	}
	queue := NewQueue()
	queue.Push("first")
	queue.Push("second")
	if value, _ := queue.Pop(); value != "first" {
		t.Fatalf("queue is not FIFO: %q", value)
	}
}

func TestCompleteBinaryTree(t *testing.T) {
	tree := NewCompleteBinaryTree()
	for i := 1; i <= 10; i++ {
		tree.Insert(strconv.Itoa(i))
	}
	if !tree.IsComplete() || !tree.Contains("7") {
		t.Fatal("complete tree invariant or search failed")
	}
	if !reflect.DeepEqual(tree.Values(), []string{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}) {
		t.Fatalf("unexpected BFS order: %v", tree.Values())
	}
}

func TestHashTables(t *testing.T) {
	for _, table := range []*OpenHashTable{NewLinearHashTable(5), NewDoubleHashTable(5)} {
		table.Set(1, 10)
		table.Set(1001, 20)
		table.Set(2001, 30)
		table.Set(1001, 25)
		if value, ok := table.Get(1001); !ok || value != 25 {
			t.Fatalf("get after collision/update failed: %d %v", value, ok)
		}
		if !table.Delete(1) || table.Delete(1) {
			t.Fatal("delete result is incorrect")
		}
		if value, ok := table.Get(2001); !ok || value != 30 {
			t.Fatal("deleted cell broke the probe sequence")
		}
	}
}

func TestTextAndBinarySerialization(t *testing.T) {
	original := NewArray()
	original.PushBack("one")
	original.PushBack("two words")
	textFile := filepath.Join(t.TempDir(), "array.json")
	binaryFile := filepath.Join(t.TempDir(), "array.gob")
	if err := SaveText(original, textFile); err != nil {
		t.Fatal(err)
	}
	if err := SaveBinary(original, binaryFile); err != nil {
		t.Fatal(err)
	}
	for _, test := range []struct {
		name string
		load func(Container, string) error
		file string
	}{
		{"text", LoadText, textFile},
		{"binary", LoadBinary, binaryFile},
	} {
		t.Run(test.name, func(t *testing.T) {
			restored := NewArray()
			if err := test.load(restored, test.file); err != nil {
				t.Fatal(err)
			}
			if !reflect.DeepEqual(restored.Values(), original.Values()) {
				t.Fatalf("restored values differ: %v", restored.Values())
			}
		})
	}
}

func BenchmarkArrayPushBack(b *testing.B) {
	array := NewArray()
	for i := 0; i < b.N; i++ {
		array.PushBack(strconv.Itoa(i))
	}
}

func BenchmarkLinearHashSet(b *testing.B) {
	table := NewLinearHashTable(b.N * 2)
	for i := 0; i < b.N; i++ {
		table.Set(i*7919, i)
	}
}

func BenchmarkDoubleHashSet(b *testing.B) {
	table := NewDoubleHashTable(b.N * 2)
	for i := 0; i < b.N; i++ {
		table.Set(i*7919, i)
	}
}

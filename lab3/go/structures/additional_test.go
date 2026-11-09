package structures

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/stretchr/testify/require"
)

func TestArrayBoundsAndAccess(t *testing.T) {
	a := NewArray()
	a.PushBack("x")
	require.Error(t, a.Insert(-1, "bad"))
	require.Error(t, a.Erase(1))
	require.Equal(t, 0, a.Find("x"))
	require.Equal(t, -1, a.Find("missing"))
	value, err := a.At(0)
	require.NoError(t, err)
	require.Equal(t, "x", value)
	a.Clear()
	require.Zero(t, a.Size())
}

func TestListBoundariesAndLinks(t *testing.T) {
	s := NewSinglyList()
	require.Error(t, s.Insert(1, "invalid"))
	require.Error(t, s.Erase(-1))
	require.NoError(t, s.Insert(0, "a"))
	require.NoError(t, s.Insert(1, "c"))
	require.NoError(t, s.Insert(1, "b"))
	require.NoError(t, s.Erase(0))
	require.NoError(t, s.Erase(1))
	require.NoError(t, s.Erase(0))
	require.Zero(t, s.Size())
	s.PushBack("next")
	require.Equal(t, []string{"next"}, s.Values())
	s.Clear()
	require.Zero(t, s.Size())
	_, err := s.nodeAt(-1)
	require.Error(t, err)

	d := NewDoublyList()
	require.Error(t, d.Insert(1, "invalid"))
	require.Error(t, d.Erase(0))
	for i, value := range []string{"a", "c", "d", "e"} {
		require.NoError(t, d.Insert(i, value))
	}
	require.NoError(t, d.Insert(1, "b"))
	require.NoError(t, d.Erase(0))
	require.NoError(t, d.Erase(3))
	require.NoError(t, d.Erase(1))
	require.Equal(t, []string{"b", "d"}, d.Values())
	require.Equal(t, []string{"d", "b"}, d.ReverseValues())
	require.Equal(t, 2, d.Size())
	d.Clear()
	require.Zero(t, d.Size())
}

func TestEmptyAndRefill(t *testing.T) {
	s := NewStack()
	_, err := s.Pop()
	require.Error(t, err)
	s.Push("top")
	v, err := s.Top()
	require.NoError(t, err)
	require.Equal(t, "top", v)
	require.Equal(t, 1, s.Size())
	s.Clear()
	require.Zero(t, s.Size())

	q := NewQueue()
	_, err = q.Pop()
	require.Error(t, err)
	q.Push("one")
	v, err = q.Pop()
	require.NoError(t, err)
	require.Equal(t, "one", v)
	q.Push("two")
	require.Equal(t, 1, q.Size())
	require.Equal(t, []string{"two"}, q.Values())
	q.Clear()
	require.Zero(t, q.Size())

	tree := NewCompleteBinaryTree()
	require.True(t, tree.IsComplete())
	require.False(t, tree.Contains("absent"))
	tree.Insert("root")
	require.Equal(t, 1, tree.Size())
	tree.Clear()
	require.Empty(t, tree.Values())
	tree.root = &treeNode{value: "root", right: &treeNode{value: "gap"}}
	require.False(t, tree.IsComplete())
}

func TestHashGrowthReuseAndExtremeKeys(t *testing.T) {
	minInt := -int(^uint(0)>>1) - 1
	for _, table := range []*OpenHashTable{NewLinearHashTable(1), NewDoubleHashTable(1), new(OpenHashTable)} {
		table.Set(minInt, 17)
		for i := 0; i < 90; i++ {
			table.Set(i, i+1)
		}
		v, ok := table.Get(minInt)
		require.True(t, ok)
		require.Equal(t, 17, v)
		for i := 0; i < 90; i++ {
			v, ok = table.Get(i)
			require.True(t, ok)
			require.Equal(t, i+1, v)
			require.True(t, table.Delete(i))
		}
		require.Equal(t, 1, table.Size())
		table.Clear()
		require.Empty(t, table.Values())
	}
}

func TestSerializationForAllStructures(t *testing.T) {
	factories := []func() Container{
		func() Container { return NewArray() },
		func() Container { return NewSinglyList() },
		func() Container { return NewDoublyList() },
		func() Container { return NewStack() },
		func() Container { return NewQueue() },
		func() Container { return NewCompleteBinaryTree() },
		func() Container { return NewLinearHashTable(17) },
		func() Container { return NewDoubleHashTable(17) },
	}
	for _, factory := range factories {
		original := factory()
		t.Run(original.Type(), func(t *testing.T) {
			stored := storedContainer{Type: original.Type(), Values: []string{"1=10", "2=20"}}
			require.NoError(t, restore(original, stored))
			for _, pair := range []struct {
				save func(Container, string) error
				load func(Container, string) error
			}{{SaveText, LoadText}, {SaveBinary, LoadBinary}} {
				path := filepath.Join(t.TempDir(), "data")
				require.NoError(t, pair.save(original, path))
				got := factory()
				require.NoError(t, pair.load(got, path))
				require.ElementsMatch(t, original.Values(), got.Values())
				if _, isHash := original.(*OpenHashTable); !isHash {
					require.Equal(t, original.Values(), got.Values())
				}
			}
		})
	}
}

func TestInvalidLoadDoesNotOverwriteExistingState(t *testing.T) {
	table := NewLinearHashTable(17)
	table.Set(7, 70)
	for _, values := range [][]string{{"1=10", "invalid"}, {"x=10"}, {"1=x"}} {
		require.Error(t, restore(table, storedContainer{Type: table.Type(), Values: values}))
		require.Equal(t, []string{"7=70"}, table.Values())
	}
	require.Error(t, restore(table, storedContainer{Type: "Array"}))
	dir := t.TempDir()
	missing := filepath.Join(dir, "missing", "file")
	require.Error(t, SaveText(table, missing))
	require.Error(t, SaveBinary(table, missing))
	require.Error(t, LoadText(table, missing))
	require.Error(t, LoadBinary(table, missing))
	bad := filepath.Join(dir, "bad")
	require.NoError(t, os.WriteFile(bad, []byte("not a file"), 0600))
	require.Error(t, LoadText(table, bad))
	require.Error(t, LoadBinary(table, bad))
}

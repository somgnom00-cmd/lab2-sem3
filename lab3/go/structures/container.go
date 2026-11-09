package structures

// Container is the common interface used for dynamic dispatch in the Go project.
type Container interface {
	Type() string
	Size() int
	Values() []string
	Clear()
}

type serializable interface {
	Container
	appendSerialized(string) error
}
